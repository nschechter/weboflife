open Webapi.Canvas;

let drawRows = (~canvasWidth, ~canvasHeight, ~tileHeight, context) => {
  let rec aux = (~currentHeight) =>
    if (currentHeight +. tileHeight > canvasHeight) {
      ();
    } else {
      Canvas2d.moveTo(~x=0.0, ~y=currentHeight, context);
      Canvas2d.lineTo(~x=canvasWidth, ~y=currentHeight, context);

      aux(~currentHeight=currentHeight +. tileHeight);
    };
  aux(~currentHeight=0.0);
};

let drawColumns = (~canvasWidth, ~canvasHeight, ~tileWidth, context) => {
  let rec aux = (~currentWidth) =>
    if (currentWidth +. tileWidth > canvasWidth) {
      ();
    } else {
      Canvas2d.moveTo(~x=currentWidth, ~y=0.0, context);
      Canvas2d.lineTo(~x=currentWidth, ~y=canvasHeight, context);

      aux(~currentWidth=currentWidth +. tileWidth);
    };
  aux(~currentWidth=0.0);
};

let isCellAlive = (~rgbTolerance, dataArr) => {
  open Js.Typed_array.Uint8ClampedArray;
  let rec aux = (~currIndex, ~darkPixels, ~lightPixels) =>
    if (currIndex + 4 >= length(dataArr)) {
      darkPixels > lightPixels;
    } else {
      switch (
        unsafe_get(dataArr, currIndex) > rgbTolerance,
        unsafe_get(dataArr, currIndex + 1) > rgbTolerance,
        unsafe_get(dataArr, currIndex + 2) > rgbTolerance,
      ) {
      | (true, true, true)
      | (true, true, false)
      | (true, false, true)
      | (false, true, true) =>
        aux(
          ~currIndex=currIndex + 4,
          ~darkPixels,
          ~lightPixels=lightPixels + 1,
        )
      | (_, _, _) =>
        aux(
          ~currIndex=currIndex + 4,
          ~darkPixels=darkPixels + 1,
          ~lightPixels,
        )
      };
    };

  aux(~currIndex=0, ~darkPixels=0, ~lightPixels=0);
};

let drawGrid = (~canvasWidth, ~canvasHeight, ~tileWidth, ~tileHeight, canvas) => {
  let context = CanvasElement.getContext2d(canvas);

  Canvas2d.beginPath(context);

  drawColumns(~canvasWidth, ~canvasHeight, ~tileWidth, context);
  drawRows(~canvasWidth, ~canvasHeight, ~tileHeight, context);

  Canvas2d.stroke(context);
};

let setWrapperPosition = ele => {
  ele
  |> Webapi.Dom.Element.asHtmlElement
  |> (
    fun
    | Some(css) =>
      Webapi.Dom.CssStyleDeclaration.setCssText(
        Webapi.Dom.HtmlElement.style(css),
        "position:fixed;width:100%;height:100%;bottom:0;left:0;right:0;top:0;z-index:9999999;background-color:white;overflow:scroll;",
      )
    | None => ()
  );
};