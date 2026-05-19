(* This code tries to translate the code given in FDL Nipkow Chapter 8. *)
type color = Red | Black

type 'a rbt =
    | Empty (* Empty tree to determine end of tree. *)
    | Node of 'a rbt * ('a * color) * 'a rbt

let get_color tree = match tree with
    | Empty -> Black
    | Node(_, (_, c), _) -> c

let paint c tree = match tree with
    | Empty -> Empty
    | Node(l, (a, _), r) -> Node(l, (a, c), r)

(* INSERT *)

let baliL left mark right = match (left, mark, right) with
    (* (R (R t1 a t2) b t3) c t4 *)
    | (Node(Node(t1, (a, Red), t2), (b, Red), t3), c, t4)
        -> Node(Node(t1, (a, Black), t2), (b, Red), Node(t3, (c, Black), t4))
    (* (R t1 a (R t2 b t3)) c t4 *)
    | (Node(t1, (a, Red), Node(t2, (b, Red), t3)), c, t4)
        -> Node(Node(t1, (a, Black), t2), (b, Red), Node(t3, (c, Black), t4))
    | (t1, a, t2) -> Node(t1, (a, Black), t2)

let baliR left mark right = match (left, mark, right) with
    (* t1 a (R t2 b (R t3 c t4)) *)
    | (t1, a, Node(t2, (b, Red), Node(t3, (c, Red), t4)))
        -> Node(Node(t1, (a, Black), t2), (b, Red), Node(t3, (c, Black), t4))
    (* t1 a (R (R t2 b t3) c t4) *)
    | (t1, a, Node(Node(t2, (b, Red), t3), (c, Red), t4))
        -> Node(Node(t1, (a, Black), t2), (b, Red), Node(t3, (c, Black), t4))
    | (t1, a, t2) -> Node(t1, (a, Black), t2)

(* This is done for better readability and to better match the code in the book. *)
type order = LT | EQ | GT

let cmp x a = let c = compare x a in 
    if c < 0 then LT 
    else if c = 0 then EQ
    else GT

let rec ins x tree = match tree with
    | Empty -> Node(Empty, (x, Red), Empty)
    | Node(l, (a, Black), r) -> (match cmp x a with
        | LT -> baliL (ins x l) a r 
        | EQ -> Node(l, (a, Black), r)
        | GT -> baliR l a (ins x r))
    | Node(l, (a, Red), r) -> (match cmp x a with
        | LT -> Node((ins x l), (a, Red), r)
        | EQ -> Node(l, (a, Red), r)
        | GT -> Node(l, (a, Red), (ins x r)))

let insert x tree = paint Black (ins x tree)

(* DELETE *)

(* TODO: Invariants for testing *)


(* Output functions. Just used to display trees. No logic. *)
let color_to_string c = match c with
    | Red -> "Red"
    | Black -> "Black"

let rec tree_to_string tree = match tree with
    | Empty -> "<>"
    | Node(l, (a, c), r) -> "Node (" ^ (tree_to_string l) ^ "<" ^ (string_of_int a) ^ "," ^ (color_to_string c) ^ ">" ^ (tree_to_string r) ^ ")"

(* To be able to use this function on trees of any type, a string conversion function 'to_string' needs to be given as well.
   For example for int rbt use 'to_string' = 'string_of_int'. *)
let rec tree_to_ascii to_string tree indent = match tree with
  | Empty -> ()
  | Node (l, (a, c), r) ->
      tree_to_ascii (to_string) r (indent ^ "      ");
      Printf.printf "%s%s(%s)\n" indent (to_string a) (color_to_string c);
      tree_to_ascii (to_string) l (indent ^ "      ")
