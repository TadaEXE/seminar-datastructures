(* Just copied code from Christoph *)

type 'a action = Insert of 'a | Delete of 'a | Find of 'a

let rec rev_rec a1 a2 = match a1 with
  [] -> a2 |
  x :: xs -> rev_rec xs (x::a2)

let rev xs = rev_rec xs []

let parse_entry text =
  try Some (Scanf.sscanf text " %[idf]%d%[^ ]" (fun qual x rest ->
    match qual with
    | "i" -> (Insert x, rest)
    | "d" -> (Delete x, rest)
    | "f" -> (Find x, rest)
    | _   -> (Insert x, rest)
  ))
  with _ -> None


let rec parse_segment text prev = let res = parse_entry text in
  match res with
    None -> (rev prev) |
    Some (act, rest) -> parse_segment rest (act :: prev)

let read_segment ic = try parse_segment (input_line ic) []
  with End_of_file -> []


