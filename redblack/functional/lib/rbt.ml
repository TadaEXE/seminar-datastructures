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
(* deletion-by-replacing approach *)

let baldL left mark right = match (left, mark, right) with
    (* (R t1 a t2) b t3 *)
    | (Node(t1, (a, Red), t2), b, t3)
        -> Node(Node(t1, (a, Black), t2), (b, Red), t3)
    (* t1 a (B t2  t3) *)
    | (t1, a, Node(t2, (b, Black), t3)) 
        -> baliR t1 a (Node(t2, (b, Red), t3))
    (* t1 a (R (B t2 b t3) c t4) *)
    | (t1, a, Node(Node(t2, (b, Black), t3), (c, Red), t4)) 
        -> Node(Node(t1, (a, Black), t2), (b, Red), (baliR t3 c (paint Red t4))) 
    | (t1, a, t2)
        -> Node(t1, (a, Red), t2) 

let baldR left mark right = match (left, mark, right) with
    (* t1 a (R t2 b t3) *)
    | (t1, a, Node(t2, (b, Red), t3))
        -> Node(t1, (a, Red), Node(t2, (b, Black), t3))
    (* (B t1 a t2) b t3 *)
    | (Node(t1, (a, Black), t2), b, t3)
        -> baliL (Node(t1, (a, Red), t2)) b t3
    (* (R t1 a (B t2 b t3)) c t4 *)
    | (Node(t1, (a, Red), Node(t2, (b, Black), t3)), c, t4)
        -> Node((baliL (paint Red t1) a t2), (b, Red), (Node(t3, (c, Black), t4)))
    | (t1, a, t2)
        -> Node(t1, (a, Red), t2)

let rec split_min tree = match tree with
    | Empty 
        -> raise (Failure "This function should not be called on a empty tree.")
    | Node(l, (a, _), r)
        -> (match l with
            | Empty -> (a, r)
            | _ -> let (x, l2) = split_min l
                in (x, if get_color l = Black then (baldL l2 a r)
                    else Node(l2, (a, Red), r)))

let rec del_replace x tree = match tree with
    | Empty -> Empty
    | Node(l, (a, _), r) -> (match (cmp x a) with
        | LT -> let l2 = (del_replace x l) in (match (l, get_color l) with
        (* if l != Empty AND color l = Black then bald l2 a r else R l2 a r *)
            | (Node _, Black) 
                -> baldL l2 a r 
            | _ -> Node(l2, (a, Red), r))
        | EQ -> (match r with
        (* if r = Empty then l else let (a2, r2) = split_min r
           in if color r = Black then baldR l a2 r2 else R l a2 r2 *)
            | Empty -> l
            | _ -> let (a2, r2) = (split_min r) in 
                if get_color r = Black then (baldR l a2 r2)
                else Node(l, (a2, Red), r2))
        | GT -> let r2 = (del_replace x r) in (match (r, get_color r) with
        (* if r != Empty AND color r = Black then baldR l a r2 else R l a r2 *)
            | (Node _, Black) -> baldR l a r2
            | _ -> Node(l, (a, Red), r2)))

let delete x tree = paint Black (del_replace x tree)

(* deletion-by-joining approach *)

let rec join tree1 tree2 = match (tree1, tree2) with
    | (Empty, t) -> t 
    | (t, Empty) -> t
    | (Node(t1, (a, Red), t2), Node(t3, (c, Red), t4))
        -> (match (join t2 t3) with
            | Node(u2, (b, Red), u3)
                -> Node(Node(t1, (a, Red), u2), (b, Red), Node(u3, (c, Red), t4))
            | t23 -> Node(t1, (a, Red), Node(t23, (c, Red), t4)))
    | (Node(t1, (a, Black), t2), Node(t3, (c, Black), t4))
        -> (match (join t2 t3) with
            | Node(u2, (b, Red), u3)
                -> Node(Node(t1, (a, Black), u2), (b, Red), Node(u3, (c, Black), t4))
            | t23 -> baldL t1 a (Node(t23, (c, Black), t4)))
    | (t1, Node(t2, (a, Red), t3))
        -> Node((join t1 t2), (a, Red), t3)
    | (Node(t1, (a, Red), t2), t3)
        -> Node(t1, (a, Red), (join t2 t3))

let rec del_join x tree = match tree with
    | Empty -> Empty
    | Node(l, (a, _), r)
        -> (match (cmp x a) with
            | LT -> (match (l, get_color l) with
                | (Node _, Black)
                    -> (baldL (del_join x l) a r)
                | _ -> Node((del_join x l), (a, Red), r))
            | EQ -> join l r
            | GT -> (match (r, get_color r) with
                | (Node _, Black)
                    -> (baldR l a (del_join x r))
                | _ -> Node(l, (a, Red), (del_join x r))))

let delete_join x tree = paint Black (del_join x tree)

(* LOOKUP *)

let rec lookup tree x = match tree with
    | Empty -> None
    | Node(l, (a, _), r) -> (match (cmp x a) with
        | LT -> lookup l x
        | EQ -> Some a
        | GT -> lookup r x)

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
