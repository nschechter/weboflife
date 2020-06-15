open Webapi.Dom;
open Webapi.Canvas;

[@bs.val] external document: Dom.document = "document";
[@bs.val] external body: Dom.element = "document.body";
[@bs.val] external innerHeight: int = "innerHeight";
[@bs.val] external innerWidth: int = "innerWidth";
[@bs.val] external pageYOffset: int = "pageYOffset";
[@bs.val] external pageXOffset: int = "pageXOffset";

let options =
  Some(
    HTML2Canvas.options(
      ~allowTaint=false,
      ~width=innerWidth,
      ~height=innerHeight,
      ~x=pageXOffset,
      ~y=pageYOffset,
      (),
    ),
  );

let stopGame = () => {
  switch (Document.querySelector("#web-of-life", document)) {
  | Some(element) => body |> Element.removeChild(element) |> ignore
  | None => ()
  };
  Game.stopGame();
};

let startGame = (gameConfig: Chrome.gameConfig) => {
  let canvasWrapper = document |> Document.createElement("div");

  HTML2Canvas.make(body, options)
  |> Js.Promise.then_(canvas => {
       CanvasUtils.setWrapperPosition(canvasWrapper);
       Element.appendChild(canvas, canvasWrapper);
       Element.appendChild(canvasWrapper, body);
       Element.setAttribute("id", "web-of-life", canvasWrapper);

       canvas |> Js.Promise.resolve;
     })
  |> Js.Promise.then_(c => {
       let canvasWidth = float_of_int(CanvasElement.width(c));
       let canvasHeight = float_of_int(CanvasElement.height(c));
       let tileWidth = canvasWidth /. float_of_int(gameConfig.columns);
       let tileHeight = canvasHeight /. float_of_int(gameConfig.rows);

       if (tileWidth >= 2.0 && tileHeight >= 2.0) {
         gameConfig.showGrid
           ? CanvasUtils.drawGrid(
               ~canvasWidth,
               ~canvasHeight,
               ~tileWidth,
               ~tileHeight,
               c,
             )
           : ignore();

         c
         |> Game.make(~gameConfig, ~tileWidth, ~tileHeight)
         |> Js.Promise.resolve;
       } else {
         stopGame();
         let message = Js.Dict.empty();
         Js.Dict.set(message, "status", "The grid is too big!");
         Chrome.Storage.set(message) |> Js.Promise.resolve;
       };
     })
  |> ignore;
};

// For debugging locally using index.html. Make sure to disable Chrome.* usage in other files
// startGame({timeInterval: 10, rows: 100, columns: 100, rgbTolerance: 235});

Chrome.Runtime.onMessageAddListener(message => {
  switch (message) {
  | StartGame(gameConfig) => startGame(gameConfig)
  | StopGame => stopGame()
  }
});