Chrome.executeScript(
  Js.Nullable.null, {file: "dist/content_script.bundle.js"}, () =>
  ReactDOMRe.renderToElementWithId(<Popup />, "container")
);