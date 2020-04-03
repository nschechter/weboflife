[@bs.val] external document: Dom.document = "document";

let createCanvas = () => {
  document |> Webapi.Dom.Document.createElement("canvas");
};

let createCanvasContext = () => {
  createCanvas() |> Webapi.Canvas.CanvasElement.getContext2d;
};