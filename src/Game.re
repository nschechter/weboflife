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
  currentAlive: int,
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
  let ctx = Webapi.Canvas.CanvasElement.getContext2d(canvas);

  let newGrid =
    Array.make_matrix(
      gameConfig.rows,
      gameConfig.columns,
      {imgData: None, state: Dead},
    );

  let rec traverseRows = (~aliveCount, ~currX, ~currY, ~maxCols) =>
    if (currX + 1 > maxCols) {
      aliveCount;
    } else {
      let data =
        countNeighbors(~grid=prevGeneration.grid, ~point=(currX, currY));
      switch (
        prevGeneration.grid[currX][currY].state,
        data.neighbors >= 2 && data.neighbors <= 3,
        data.neighbors == 3,
        data.lastNeighborImgData,
      ) {
      | (Alive, true, _, _) =>
        newGrid[currX][currY] = prevGeneration.grid[currX][currY];
        traverseRows(
          ~aliveCount=aliveCount + 1,
          ~currX=currX + 1,
          ~currY,
          ~maxCols,
        );
      | (Alive, false, _, _) =>
        newGrid[currX][currY] = {imgData: None, state: Dead};
        let emptyData =
          Webapi.Canvas.Canvas2d.createImageDataCoords(
            ~width=tileWidth -. 1.0,
            ~height=tileHeight -. 1.0,
            ctx,
          );
        Webapi.Canvas.Canvas2d.putImageData(
          ctx,
          ~imageData=emptyData,
          ~dx=float_of_int(currX) *. tileWidth +. 1.0,
          ~dy=float_of_int(currY) *. tileHeight +. 1.0,
          ~dirtyX=0.0,
          ~dirtyY=0.0,
          ~dirtyWidth=tileWidth,
          ~dirtyHeight=tileHeight,
          (),
        );
        traverseRows(
          ~aliveCount=aliveCount + 1,
          ~currX=currX + 1,
          ~currY,
          ~maxCols,
        );
      | (Dead, _, true, Some(imgData)) =>
        newGrid[currX][currY] = {imgData: Some(imgData), state: Alive};
        Webapi.Canvas.Canvas2d.putImageData(
          ctx,
          ~imageData=imgData,
          ~dx=float_of_int(currX) *. tileWidth,
          ~dy=float_of_int(currY) *. tileHeight,
          ~dirtyX=0.0,
          ~dirtyY=0.0,
          ~dirtyWidth=tileWidth,
          ~dirtyHeight=tileHeight,
          (),
        );
        traverseRows(~aliveCount, ~currX=currX + 1, ~currY, ~maxCols);
      | (_, _, _, _) =>
        newGrid[currX][currY] = {imgData: None, state: Dead};
        traverseRows(~aliveCount, ~currX=currX + 1, ~currY, ~maxCols);
      };
    };

  let rec traverseColumns = (~aliveCount, ~currY, ~maxRows) =>
    if (currY + 1 > maxRows) {
      {
        grid: newGrid,
        currentAlive: aliveCount,
        tileWidth,
        tileHeight,
        canvas,
      };
    } else {
      let amtAliveInRow =
        traverseRows(
          ~aliveCount,
          ~currY,
          ~currX=0,
          ~maxCols=gameConfig.columns,
        );
      traverseColumns(
        ~aliveCount=amtAliveInRow,
        ~currY=currY + 1,
        ~maxRows=gameConfig.rows,
      );
    };

  traverseColumns(~aliveCount=0, ~currY=0, ~maxRows=gameConfig.rows);
};

let createGeneration =
    (~gameConfig: Chrome.gameConfig, ~tileWidth, ~tileHeight, ~canvas) => {
  let ctx = Webapi.Canvas.CanvasElement.getContext2d(canvas);

  let grid =
    Array.make_matrix(
      gameConfig.rows,
      gameConfig.columns,
      {imgData: None, state: Dead},
    );

  let rec traverseRows = (~aliveCount, ~currX, ~currY, ~maxCols) =>
    if (currX + 1 > maxCols) {
      aliveCount;
    } else {
      let imgData =
        Webapi.Canvas.Canvas2d.getImageData(
          ~sx=float_of_int(currX) *. tileWidth,
          ~sy=float_of_int(currY) *. tileHeight,
          ~sw=tileWidth,
          ~sh=tileHeight,
          ctx,
        );

      CanvasUtils.isCellAlive(
        ~rgbTolerance=gameConfig.rgbTolerance,
        Webapi.Dom.Image.data(imgData),
      )
        ? {
          grid[currX][currY] = {imgData: Some(imgData), state: Alive};
          traverseRows(
            ~aliveCount=aliveCount + 1,
            ~currX=currX + 1,
            ~currY,
            ~maxCols,
          );
        }
        : {
          grid[currX][currY] = {imgData: None, state: Dead};
          traverseRows(~aliveCount, ~currX=currX + 1, ~currY, ~maxCols);
        };
    };

  let rec traverseColumns = (~aliveCount, ~currY, ~maxRows) =>
    if (currY + 1 > maxRows) {
      {grid, currentAlive: aliveCount, tileWidth, tileHeight, canvas};
    } else {
      let amtAliveInRow =
        traverseRows(
          ~aliveCount,
          ~currY,
          ~currX=0,
          ~maxCols=gameConfig.columns,
        );
      traverseColumns(
        ~aliveCount=amtAliveInRow,
        ~currY=currY + 1,
        ~maxRows=gameConfig.rows,
      );
    };

  traverseColumns(~aliveCount=0, ~currY=0, ~maxRows=gameConfig.columns);
};

let stopGame = () => {
  isGameRunning := false;
};

let make = (~gameConfig: Chrome.gameConfig, ~tileWidth, ~tileHeight, canvas) => {
  let timeoutId = ref(None);
  let port = Chrome.Runtime.connect({name: "aliveCount"});

  let rec loop = currentGen =>
    if (currentGen.currentAlive == 0 || ! isGameRunning^) {
      switch (timeoutId^) {
      | Some(id) => Js.Global.clearTimeout(id)
      | None => ignore()
      };
    } else {
      let nextGen =
        currentGen
        |> nextGeneration(~gameConfig, ~canvas, ~tileWidth, ~tileHeight);

      Chrome.Port.postMessage(port, string_of_int(nextGen.currentAlive));

      timeoutId :=
        Some(
          Js.Global.setTimeout(_ => loop(nextGen), gameConfig.timeInterval),
        );
    };

  isGameRunning := true;
  createGeneration(~gameConfig, ~tileWidth, ~tileHeight, ~canvas) |> loop;
};