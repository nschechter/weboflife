open Chrome;

type action =
  | SetInterval(string)
  | SetRows(string)
  | SetColumns(string);

type state = {
  interval: string,
  rows: string,
  columns: string,
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
        },
      {interval: "1000", rows: "10", columns: "10"},
    );

  let handleStartGame =
    React.useCallback1(
      _ =>
        query({currentWindow: Some(true), active: Some(true)}, tabs =>
          tabs[0].id
          |> (
            fun
            | Some(id) => {
                sendMessage(
                  id,
                  StartGame({
                    timeInterval: int_of_string(state.interval),
                    rows: int_of_string(state.rows),
                    columns: int_of_string(state.columns),
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
      query({currentWindow: Some(true), active: Some(true)}, tabs =>
        tabs[0].id
        |> (
          fun
          | Some(id) => sendMessage(id, StopGame)
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

  <div className="Popup">
    <button onClick=handleStartGame> {React.string("Start")} </button>
    <button onClick=handleEndGame> {React.string("Stop")} </button>
    <input
      type_="number"
      placeholder="Interval"
      value={state.interval}
      onChange=handleChangeInterval
      min=1
    />
    <input
      type_="number"
      placeholder="Rows"
      value={state.rows}
      onChange=handleChangeRows
      min=1
    />
    <input
      type_="number"
      placeholder="Columns"
      value={state.columns}
      onChange=handleChangeColumns
      min=1
    />
  </div>;
};