type queryInfo = {
  currentWindow: option(bool),
  active: option(bool),
};

type tab = {
  id: option(int),
  active: bool,
};

type gameConfig = {
  timeInterval: int,
  rows: int,
  columns: int,
  rgbTolerance: int,
  showGrid: bool,
};

type executeScriptOptions = {file: string};

type message =
  | StartGame(gameConfig)
  | StopGame;

module Port = {
  type t;

  type connectOptions = {name: string};

  [@bs.send] external postMessage: (t, string) => unit = "postMessage";

  [@bs.send] [@bs.scope "onMessage"]
  external onMessageAddListener: (t, string => unit) => unit = "addListener";
};

module Runtime = {
  [@bs.val]
  external onMessageAddListener: (message => unit) => unit =
    "chrome.runtime.onMessage.addListener";

  [@bs.val]
  external onConnectAddListener: (Port.t => unit) => unit =
    "chrome.runtime.onConnect.addListener";

  [@bs.val]
  external connect: Port.connectOptions => Port.t = "chrome.runtime.connect";
};

module Tabs = {
  [@bs.val]
  external sendMessage: (int, message) => unit = "chrome.tabs.sendMessage";

  [@bs.val]
  external query: (queryInfo, array(tab) => unit) => unit =
    "chrome.tabs.query";

  [@bs.val]
  external executeScript:
    (Js.Nullable.t(int), executeScriptOptions, unit => unit) => unit =
    "chrome.tabs.executeScript";
};