Misc.import("./Popup.scss");

type action =
  | SetInterval(string)
  | SetRows(string)
  | SetColumns(string)
  | SetRGBTolerance(string)
  | SetAlive(string);

type state = {
  interval: string,
  rows: string,
  columns: string,
  rgbTolerance: string,
  alive: string,
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
        },
      {
        interval: "1000",
        rows: "10",
        columns: "10",
        rgbTolerance: "235",
        alive: "0",
      },
    );

  React.useEffect0(() => {
    // Chrome.Runtime.connect({name: "aliveCount "})
    // ->Chrome.Port.onMessageAddListener(newAliveCount =>
    //     currentAlive := newAliveCount
    //   );
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
    <span> {React.string(state.alive)} </span>
  </div>;
};