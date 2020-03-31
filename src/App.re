open Webapi.Dom;
open Webapi.Canvas;
open Chrome;

[@bs.val] external document: Dom.document = "document";
[@bs.val] external body: Dom.element = "document.body";

let options = Some(HTML2Canvas.options(~allowTaint=false, ()));

let startGame = gameConfig => {
  let canvasWrapper = document |> Document.createElement("div");
  Element.setAttribute("id", "web-of-life", canvasWrapper);

  HTML2Canvas.make(body, options)
  |> Js.Promise.then_(c => {
       canvasWrapper |> Element.appendChild(c);
       c |> Js.Promise.resolve;
     })
  |> Js.Promise.then_(c => {
       canvasWrapper |> Canvas.setWrapperPosition;

       body |> Element.appendChild(canvasWrapper);
       c |> Js.Promise.resolve;
     })
  |> Js.Promise.then_(c => {
       let canvasWidth = float_of_int(CanvasElement.width(c));
       let canvasHeight = float_of_int(CanvasElement.height(c));
       let tileWidth = canvasWidth /. float_of_int(gameConfig.columns);
       let tileHeight = canvasHeight /. float_of_int(gameConfig.rows);

       Canvas.drawGrid(
         ~canvasWidth,
         ~canvasHeight,
         ~tileWidth,
         ~tileHeight,
         c,
       );

       c
       |> Game.make(~gameConfig, ~tileWidth, ~tileHeight)
       |> Js.Promise.resolve;
     })
  |> ignore;
};

onMessageAddListener(message => {
  switch (message) {
  | StartGame(gameConfig) => startGame(gameConfig)
  | StopGame =>
    switch (Document.querySelector("#web-of-life", document)) {
    | Some(element) => body |> Element.removeChild(element) |> ignore
    | None => ()
    };
    Game.stopGame();
  }
});