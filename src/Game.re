module TupleHash =
  Belt.Id.MakeHashable({
    type t = (float, float);
    let hash = point => int_of_float(fst(point) +. 31.0 *. snd(point));
    let eq = (a, b) => fst(a) == fst(b) && snd(a) == snd(b);
  });

type aliveStates =
  | Dead
  | Alive;

type cell = {
  mutable imgData: option(Webapi__Dom__Image.t),
  state: aliveStates,
};

type game = {
  tileWidth: float,
  tileHeight: float,
  canvas: Dom.element,
  grid: array(array(cell)),
  currentAlive: ref(int),
};

type countNeighbors = {
  neighbors: int,
  mutable lastNeighborImgData: option(Webapi__Dom__Image.t),
};

let isGameRunning = ref(false);

let countNeighbors = (~grid, ~point) => {
  let rec rowTraversal =
          (~lastNeighborImgData, ~neighbors, ~currY, ~currX, ~maxRows) =>
    if (currY > maxRows) {
      {neighbors, lastNeighborImgData};
    } else if (currX == 0 && currY == 0) {
      rowTraversal(
        ~lastNeighborImgData,
        ~neighbors,
        ~currY=currY + 1,
        ~currX,
        ~maxRows,
      );
    } else {
      Belt.Array.get(grid, fst(point) + currX)
      |> (
        fun
        | Some(row) =>
          Belt.Array.get(row, snd(point) + currY)
          |> (
            fun
            | Some(cell) => {
                cell.state
                |> (
                  fun
                  | Alive =>
                    rowTraversal(
                      ~lastNeighborImgData=cell.imgData,
                      ~neighbors=neighbors + 1,
                      ~currY=currY + 1,
                      ~currX,
                      ~maxRows,
                    )
                  | Dead =>
                    rowTraversal(
                      ~lastNeighborImgData,
                      ~neighbors,
                      ~currY=currY + 1,
                      ~currX,
                      ~maxRows,
                    )
                );
              }
            | None =>
              rowTraversal(
                ~lastNeighborImgData,
                ~neighbors,
                ~currY=currY + 1,
                ~currX,
                ~maxRows,
              )
          )
        | None =>
          rowTraversal(
            ~lastNeighborImgData,
            ~neighbors,
            ~currY=currY + 1,
            ~currX,
            ~maxRows,
          )
      );
    };

  let rec colTraversal = (~lastNeighborImgData, ~neighbors, ~currX, ~maxCols) =>
    if (currX > maxCols) {
      {neighbors, lastNeighborImgData};
    } else {
      let data =
        rowTraversal(
          ~lastNeighborImgData,
          ~neighbors,
          ~currY=-1,
          ~currX,
          ~maxRows=1,
        );
      colTraversal(
        ~lastNeighborImgData=data.lastNeighborImgData,
        ~neighbors=data.neighbors,
        ~currX=currX + 1,
        ~maxCols,
      );
    };

  colTraversal(
    ~lastNeighborImgData=None,
    ~neighbors=0,
    ~currX=-1,
    ~maxCols=1,
  );
};

let nextGeneration =
    (
      ~gameConfig: Chrome.gameConfig,
      ~canvas,
      ~tileWidth,
      ~tileHeight,
      prevGeneration,
    ) => {
  open Webapi.Canvas;
  let ctx = CanvasElement.getContext2d(canvas);

  let newGrid =
    Array.make_matrix(
      gameConfig.rows,
      gameConfig.columns,
      {imgData: None, state: Dead},
    );

  for (x in 0 to gameConfig.rows - 1) {
    for (y in 0 to gameConfig.columns - 1) {
      let data = countNeighbors(~grid=prevGeneration.grid, ~point=(x, y));
      switch (
        prevGeneration.grid[x][y].state,
        data.neighbors >= 2 && data.neighbors <= 3,
        data.neighbors == 3,
        data.lastNeighborImgData,
      ) {
      | (Alive, true, _, _) => newGrid[x][y] = prevGeneration.grid[x][y]
      | (Alive, false, _, _) =>
        newGrid[x][y] = {imgData: None, state: Dead};
        let emptyData =
          Canvas2d.createImageDataCoords(
            ~width=tileWidth -. 1.0,
            ~height=tileHeight -. 1.0,
            ctx,
          );
        Canvas2d.putImageData(
          ctx,
          ~imageData=emptyData,
          ~dx=float_of_int(x) *. tileWidth +. 1.0,
          ~dy=float_of_int(y) *. tileHeight +. 1.0,
          ~dirtyX=0.0,
          ~dirtyY=0.0,
          ~dirtyWidth=tileWidth,
          ~dirtyHeight=tileHeight,
          (),
        );
      | (Dead, _, true, Some(imgData)) =>
        newGrid[x][y] = {imgData: Some(imgData), state: Alive};
        Canvas2d.putImageData(
          ctx,
          ~imageData=imgData,
          ~dx=float_of_int(x) *. tileWidth,
          ~dy=float_of_int(y) *. tileHeight,
          ~dirtyX=0.0,
          ~dirtyY=0.0,
          ~dirtyWidth=tileWidth,
          ~dirtyHeight=tileHeight,
          (),
        );
      | (_, _, _, _) => newGrid[x][y] = {imgData: None, state: Dead}
      };
    };
  };

  {grid: newGrid, currentAlive: ref(0), tileWidth, tileHeight, canvas};
};

let createGeneration =
    (~gameConfig: Chrome.gameConfig, ~tileWidth, ~tileHeight, ~canvas) => {
  open Webapi.Canvas;
  let ctx = CanvasElement.getContext2d(canvas);
  let currentAlive = ref(0);

  let grid =
    Array.make_matrix(
      gameConfig.rows,
      gameConfig.columns,
      {imgData: None, state: Dead},
    );

  for (x in 0 to gameConfig.rows - 1) {
    for (y in 0 to gameConfig.columns - 1) {
      let imgData =
        Canvas2d.getImageData(
          ~sx=float_of_int(x) *. tileWidth,
          ~sy=float_of_int(y) *. tileHeight,
          ~sw=tileWidth,
          ~sh=tileHeight,
          ctx,
        );

      Canvas.isCellAlive(imgData |> Webapi.Dom.Image.data)
        ? {
          grid[x][y] = {imgData: Some(imgData), state: Alive};
          currentAlive := currentAlive^ + 1;
        }
        : grid[x][y] = {imgData: None, state: Dead};
    };
  };

  {grid, currentAlive, tileWidth, tileHeight, canvas};
};

let make = (~gameConfig: Chrome.gameConfig, ~tileWidth, ~tileHeight, canvas) => {
  let timeoutId = ref(None);

  let rec loop = currentGen =>
    if (! isGameRunning^) {
      switch (timeoutId^) {
      | Some(id) => Js.Global.clearTimeout(id)
      | None => ()
      };
    } else {
      let nextGen =
        currentGen
        |> nextGeneration(~gameConfig, ~canvas, ~tileWidth, ~tileHeight);

      timeoutId :=
        Some(
          Js.Global.setTimeout(() => loop(nextGen), gameConfig.timeInterval),
        );
    };

  isGameRunning := true;
  createGeneration(~gameConfig, ~tileWidth, ~tileHeight, ~canvas) |> loop;
};

let stopGame = () => {
  isGameRunning := false;
};