Misc.import("./Popup.scss");

type action =
  | SetInterval(string)
  | SetRows(string)
  | SetColumns(string)
  | SetRGBTolerance(string)
  | SetAlive(string)
  | SetShowGrid(bool);

type state = {
  interval: string,
  rows: string,
  columns: string,
  rgbTolerance: string,
  alive: string,
  showGrid: bool,
};

[@react.component]
let make = () => {
  let (state, dispatch) =
    React.useReducer(
      (state, action) =>
        switch (action) {
        | SetInterval(interval) => {...state, interval}
        | SetRows(rows) => {...state, rows}
        | SetColumns(columns) => {...state, columns}
        | SetRGBTolerance(rgbTolerance) => {...state, rgbTolerance}
        | SetAlive(aliveCount) => {...state, alive: aliveCount}
        | SetShowGrid(val_) => {...state, showGrid: val_}
        },
      {
        interval: "1000",
        rows: "10",
        columns: "10",
        rgbTolerance: "235",
        alive: "0",
        showGrid: true,
      },
    );

  React.useEffect0(() => {
    Chrome.Runtime.onConnectAddListener(port =>
      Chrome.Port.onMessageAddListener(port, msg => dispatch(SetAlive(msg)))
    );
    None;
  });

  let handleStartGame =
    React.useCallback1(
      _ =>
        Chrome.Tabs.query(
          {currentWindow: Some(true), active: Some(true)}, tabs =>
          tabs[0].id
          |> (
            fun
            | Some(id) => {
                Chrome.Tabs.sendMessage(
                  id,
                  StartGame({
                    timeInterval: int_of_string(state.interval),
                    rows: int_of_string(state.rows),
                    columns: int_of_string(state.columns),
                    rgbTolerance: int_of_string(state.rgbTolerance),
                    showGrid: state.showGrid,
                  }),
                );
              }
            | None => ()
          )
        ),
      [|state|],
    );

  let handleEndGame =
    React.useCallback0(_ =>
      Chrome.Tabs.query(
        {currentWindow: Some(true), active: Some(true)}, tabs =>
        tabs[0].id
        |> (
          fun
          | Some(id) => Chrome.Tabs.sendMessage(id, StopGame)
          | None => ()
        )
      )
    );

  let handleChangeInterval =
    React.useCallback1(
      event => dispatch(SetInterval(ReactEvent.Form.target(event)##value)),
      [|state.interval|],
    );

  let handleChangeRows =
    React.useCallback1(
      event => dispatch(SetRows(ReactEvent.Form.target(event)##value)),
      [|state.rows|],
    );

  let handleChangeColumns =
    React.useCallback1(
      event => dispatch(SetColumns(ReactEvent.Form.target(event)##value)),
      [|state.columns|],
    );

  let handleChangeRGBTolerance =
    React.useCallback1(
      event =>
        dispatch(SetRGBTolerance(ReactEvent.Form.target(event)##value)),
      [|state.rgbTolerance|],
    );

  let handleChangeShowGrid =
    React.useCallback1(
      event =>
        dispatch(SetShowGrid(ReactEvent.Form.target(event)##checked)),
      [|state.showGrid|],
    );

  <div className="Popup">
    <div className="Popup__top">
      <button className="Popup__button" onClick=handleStartGame>
        {React.string("Start")}
      </button>
      <button className="Popup__button" onClick=handleEndGame>
        {React.string("Stop")}
      </button>
    </div>
    <label htmlFor="Interval"> {React.string("Interval")} </label>
    <input
      className="Popup__input"
      type_="number"
      id="interval"
      placeholder="Interval"
      value={state.interval}
      onChange=handleChangeInterval
      min=1
    />
    <label htmlFor="rows"> {React.string("Rows")} </label>
    <input
      className="Popup__input"
      type_="number"
      id="rows"
      placeholder="Rows"
      value={state.rows}
      onChange=handleChangeRows
      min=1
    />
    <label htmlFor="columns"> {React.string("Columns")} </label>
    <input
      className="Popup__input"
      type_="number"
      id="columns"
      placeholder="Columns"
      value={state.columns}
      onChange=handleChangeColumns
      min=1
    />
    <label htmlFor="rgbtolerance"> {React.string("RGB Tolerance")} </label>
    <input
      className="Popup__input"
      type_="number"
      id="rgbtolerance"
      placeholder="RGB Tolerance"
      value={state.rgbTolerance}
      onChange=handleChangeRGBTolerance
      min=0
      max="255"
    />
    <label htmlFor="showgrid"> {React.string("Show Grid")} </label>
    <input
      className="Popup__input"
      type_="checkbox"
      id="showgrid"
      checked={state.showGrid}
      onChange=handleChangeShowGrid
    />
    <div> {React.string(state.alive)} </div>
  </div>;
};