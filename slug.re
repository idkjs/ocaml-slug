module Slug_data = Slug_data;

module Charmap = {
  type t = Hashtbl.t(string, string);

  let mk_charmap = maps => {
    let charmap_hash: t = (Hashtbl.create(600): t);
    let () =
      maps
      |> List.iter(
           List.iter(((key, value)) =>
             Hashtbl.add(charmap_hash, key, value)
           ),
         );

    charmap_hash;
  };

  let base = mk_charmap([Slug_data.base]);
};

let non_alphaNum = Re.Pcre.regexp({|[^A-Za-z0-9\s]|});

let slugify = (~sep="-", ~charmap=Charmap.base, ~lowercase=true, str) => {
  let str =
    Uunf_string.normalize_utf_8(`NFD, str)
    |> Uuseg_string.fold_utf_8(
         `Grapheme_cluster,
         (acc, cluster) =>
           switch (Hashtbl.find_opt(charmap, cluster)) {
           | None => acc ++ cluster
           | Some(replace) => acc ++ replace
           },
         "",
       )
    |> String.trim;
  let str = Re.replace_string(~all=true, non_alphaNum, ~by="", str);
  let str =
    Re.replace_string(~all=true, Re.Pcre.regexp("\\s+"), ~by=sep, str);
  let str =
    if (lowercase) {
      String.lowercase_ascii(str);
    } else {
      str;
    };
  str;
};
