open Belt;

module TupleHash =
  Belt.Id.MakeHashable({
    type t = (int, int);
    let hash = point => fst(point) + 31 * snd(point);
    let eq = (a, b) => fst(a) == fst(b) && snd(a) == snd(b);
  });

type game = {
  tileWidth: float,
  tileHeight: float,
  canvas: Dom.element,
  currentAlive: int,
  map: Belt.HashMap.t(TupleHash.t, Webapi__Dom__Image.t, TupleHash.identity),
};

let isGameRunning = ref(false);

let getImageData = (~tileWidth, ~tileHeight, ~currX, ~currY, ctx) =>
  Webapi.Canvas.Canvas2d.getImageData(
    ~sx=float_of_int(currX) *. tileWidth,
    ~sy=float_of_int(currY) *. tileHeight,
    ~sw=tileWidth,
    ~sh=tileHeight,
    ctx,
  );

let isInBounds = (~gameConfig: Chrome.gameConfig, ~x, ~y) => {
  let xMax = gameConfig.rows;
  let yMax = gameConfig.columns;

  x >= 0 && y >= 0 && x <= xMax && y <= yMax;
};

let nextGeneration =
    (
      ~gameConfig: Chrome.gameConfig,
      ~tileWidth,
      ~tileHeight,
      ~canvas,
      prevGen,
    ) => {
  let ctx = Webapi.Canvas.CanvasElement.getContext2d(canvas);

  let deadMap =
    HashMap.make(
      ~hintSize=gameConfig.columns * gameConfig.rows,
      ~id=(module TupleHash),
    );

  let aliveMap =
    HashMap.make(
      ~hintSize=gameConfig.columns * gameConfig.rows,
      ~id=(module TupleHash),
    );

  let newLiveMap = HashMap.copy(prevGen.map);

  HashMap.forEach(newLiveMap, (k, v) => {
    for (i in (-1) to 1) {
      for (j in (-1) to 1) {
        let coord = (fst(k) + i, snd(k) + j);
        if ((i != 0 || j != 0)
            && isInBounds(~gameConfig, ~x=fst(coord), ~y=snd(coord))) {
          if (HashMap.has(newLiveMap, coord)) {
            HashMap.get(aliveMap, coord)
            |> (
              fun
              | Some(count) => HashMap.set(aliveMap, coord, count + 1)
              | None => HashMap.set(aliveMap, coord, 1)
            );
          } else {
            HashMap.get(deadMap, coord)
            |> (
              fun
              | Some((count, data)) =>
                HashMap.set(deadMap, coord, (count + 1, data))
              | None => HashMap.set(deadMap, coord, (1, v))
            );
          };
        };
      };
    }
  });

  HashMap.forEach(aliveMap, ((x, y), neighborCount) =>
    if (neighborCount < 2 || neighborCount > 3) {
      let emptyData =
        Webapi.Canvas.Canvas2d.createImageDataCoords(
          ~width=tileWidth -. 1.0,
          ~height=tileHeight -. 1.0,
          ctx,
        );
      Webapi.Canvas.Canvas2d.putImageData(
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
      HashMap.remove(newLiveMap, (x, y));
    }
  );

  HashMap.forEach(deadMap, ((x, y), (count, imgData)) =>
    if (count == 3) {
      Webapi.Canvas.Canvas2d.putImageData(
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
      HashMap.set(newLiveMap, (x, y), imgData);
    }
  );

  {
    currentAlive: HashMap.size(newLiveMap),
    tileWidth,
    tileHeight,
    canvas,
    map: newLiveMap,
  };
};

let createGeneration =
    (~gameConfig: Chrome.gameConfig, ~tileWidth, ~tileHeight, ~canvas) => {
  let initialGen =
    HashMap.make(
      ~hintSize=gameConfig.columns * gameConfig.rows,
      ~id=(module TupleHash),
    );

  let ctx = Webapi.Canvas.CanvasElement.getContext2d(canvas);

  let rec traverseRow = (~currX, ~currY, ~maxCols) =>
    if (currX + 1 < maxCols) {
      let imgData =
        getImageData(~tileWidth, ~tileHeight, ~currX, ~currY, ctx);

      if (CanvasUtils.isCellAlive(
            ~rgbTolerance=gameConfig.rgbTolerance,
            Webapi.Dom.Image.data(imgData),
          )) {
        HashMap.set(initialGen, (currX, currY), imgData);
      };

      traverseRow(~currX=currX + 1, ~currY, ~maxCols);
    };

  let rec traverseColumn = (~currY, ~maxRows) =>
    if (currY + 1 > maxRows) {
      {
        currentAlive: HashMap.size(initialGen),
        map: initialGen,
        tileWidth,
        tileHeight,
        canvas,
      };
    } else {
      traverseRow(~currY, ~currX=0, ~maxCols=gameConfig.columns) |> ignore;
      traverseColumn(~currY=currY + 1, ~maxRows=gameConfig.rows);
    };

  traverseColumn(~currY=0, ~maxRows=gameConfig.columns);
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