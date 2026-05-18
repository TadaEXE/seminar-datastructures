(* This code tries to translate the code given in FDL Nipkow Chapter 8. *)
type color = Red | Black

type 'a rbt =
    | Empty (* Empty tree to determine end of tree. *)
    | Node of 'a rbt * ('a * color) * 'a rbt

let get_color tree = match tree with
    | Empty -> Black
    | Node(_, (_, c), _) -> c

(* The following function was only used to try get_color in main.ml *)
let color_to_string c = match c with
    | Red -> "Red"
    | Black -> "Black"

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

(* DELETE *)

(* TODO: Invariants for testing *)
