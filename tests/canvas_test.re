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

afterEach(() =>
  switch (document->Document.unsafeAsHtmlDocument->HtmlDocument.body) {
  | Some(body) => body->setInnerHTML("")
  | None => raise(Failure("Unable to find document body"))
  }
);

test("toContainCanvas", () =>
  render({|<canvas data-testid="grid"></canvas>|})
  |> queryByTestId("grid")
  |> expect
  |> JestDom.toBeVisible
);

test("isCellAlive", () =>
  render({|<button disabled data-testid="button"></button>|})
  |> queryByTestId("button")
  |> expect
  |> JestDom.toBeDisabled
);

test("not isCellAlive", () =>
  render({|<button disabled data-testid="button"></button>|})
  |> queryByTestId("button")
  |> expect
  |> JestDom.toBeDisabled
);