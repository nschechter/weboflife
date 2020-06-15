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
  external onMessageAddListenerStatus: (string => unit) => unit =
    "chrome.runtime.onMessage.addListener";

  [@bs.val]
  external onMessageAddListener: (message => unit) => unit =
    "chrome.runtime.onMessage.addListener";

  [@bs.val]
  external onConnectAddListener: (Port.t => unit) => unit =
    "chrome.runtime.onConnect.addListener";

  [@bs.val]
  external sendMessage: (Js.Dict.t(string), string => unit) => unit =
    "chrome.runtime.sendMessage";

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

module Storage = {
  [@bs.val]
  external set: Js.Dict.t(string) => unit = "chrome.storage.local.set";

  [@bs.val]
  external get: (Js.Dict.t(string), array(Js.Dict.t(string)) => unit) =
    "chrome.storage.local.get";

  [@bs.val]
  external addListener: (string => unit) => unit =
    "chrome.storage.onChanged.addListener";
};