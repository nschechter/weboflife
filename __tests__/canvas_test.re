open Jest;
open Expect;
open Webapi.Dom;
open Webapi.Dom.Element;

let render = html => {
  let body = Document.createElement("body", document);

  body->setInnerHTML(html);

  document->Document.unsafeAsHtmlDocument->HtmlDocument.setBody(body);

  body;
};

let queryByTestId = (id: string, element: Dom.element) =>
  switch (element |> querySelector({j|[data-testid="$(id)"]|j})) {
  | Some(el) => el
  | None => raise(Failure("Element not found"))
  };

afterEach(() => {
  switch (document->Document.unsafeAsHtmlDocument->HtmlDocument.body) {
  | Some(body) => body->setInnerHTML("")
  | None => raise(Failure("Unable to find document body"))
  }
});

test("toContainCanvas", () =>
  render({|<canvas data-testid="grid"></canvas>|})
  |> queryByTestId("grid")
  |> expect
  |> JestDom.toBeVisible
);

test("isCellAlive", () => {
  CanvasHelper.createCanvasContext()
  |> Webapi.Canvas.Canvas2d.createImageDataCoords(~width=10.0, ~height=10.0)
  |> Webapi.Dom.Image.data
  |> CanvasUtils.isCellAlive(~rgbTolerance=100)
  |> expect
  |> toBe(true)
});

test("not isCellAlive", () =>
  CanvasHelper.createCanvasContext()
  |> Webapi.Canvas.Canvas2d.createImageDataCoords(~width=1.0, ~height=1.0)
  |> Webapi.Dom.Image.data
  |> Js.Typed_array.Uint8ClampedArray.map((. _) => 50)
  |> CanvasUtils.isCellAlive(~rgbTolerance=40)
  |> expect
  |> toBe(false)
);