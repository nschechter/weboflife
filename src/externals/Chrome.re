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
};

type message =
  | StartGame(gameConfig)
  | StopGame;

[@bs.val]
external onMessageAddListener: (message => unit) => unit =
  "chrome.runtime.onMessage.addListener";

[@bs.val]
external sendMessage: (int, message) => unit = "chrome.tabs.sendMessage";

[@bs.val]
external query: (queryInfo, array(tab) => unit) => unit = "chrome.tabs.query";