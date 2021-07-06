let re_path = "slug_data.re";

let charmap_path = "data/charmap.json";

let locales_path = "data/locales.json";

let write_charmap = (~oc, ~variable, ~json) => {
  let buf = Buffer.create(1000);
  Printf.bprintf(buf, "let %s = [\n", variable);
  let () =
    switch (json) {
    | `Assoc(pairs) =>
      List.iter(
        ((key, value)) =>
          if (key == "locale") {
            ();
          } else {
            let value =
              switch (value) {
              | `String(value) => value
              | _ => failwith("Incorrect pair value. Expect string.")
              };
            Printf.bprintf(buf, "  ({|%s|}, {|%s|});\n", key, value);
          },
        pairs,
      )
    | _ => failwith("Incorrect json structure. Expect object.")
    };
  Buffer.add_string(buf, "]");
  Printf.fprintf(oc, "%s\n", Buffer.contents(buf));
};

let () = {
  let oc = open_out(re_path);
  Printf.fprintf(oc, "%s\n", "(* This file is auto generated *)");
  write_charmap(
    ~oc,
    ~variable="base",
    ~json=Yojson.Basic.from_file(charmap_path),
  );

  let locales_map = Yojson.Basic.from_file(locales_path);
  let () =
    switch (locales_map) {
    | `Assoc(pairs) =>
      List.iter(
        ((locale, charmap)) =>
          write_charmap(~oc, ~variable=locale, ~json=charmap),
        pairs,
      )
    | _ => failwith("Incorrect json structure. Expect object.")
    };
  close_out(oc);
};
