module Avl = struct
  type color = Red | Black
end

type 'a avl = Leaf | Node of (int * 'a * 'a avl * 'a avl)

let ht tree = 
  match tree with 
  Leaf -> 0 |
  Node (h, _, _, _) -> h

let rec avl_inv tree =
  match tree with
  Leaf -> true |
  Node (h, _, left, right) -> h = max (ht left) (ht right) + 1 &&
                              max (ht left) (ht right) <= min (ht left) (ht right) + 1 &&
                              avl_inv left && avl_inv right

let rec print_avl_rec tree = match tree with 
  Leaf -> "<>" |
  Node (h, a, left, right) -> 
            "(" ^ print_avl_rec left ^ "," ^ string_of_int h ^ "," ^ 
            string_of_int a ^ "," ^ print_avl_rec right ^ ")"

let print_avl tree = print_newline (print_string (print_avl_rec tree))

let rec find x tree =
    match x, tree with
    a, Leaf -> false |
    a, Node (h, b, left, right) -> a == b || find x left || find x right


let balL tree = match tree with
  Leaf -> tree |
  Node (h, a, Leaf, right) -> tree |
  Node (h0, c, Node (h1, a, t1, t2), t3) -> 
    if (h1 = ht t3 + 2) 
      then if ht t1 >= ht t2 
        then Node (h0 - 1, a, t1, Node (ht t2 + 1, c, t2, t3)) 
        else match t2 with Leaf -> tree |
          Node (h2, b, t21, t22) -> 
            Node (h0 - 1, b, Node (ht t1 + 1, a, t1, t21), Node (ht t3 + 1, c, t22, t3))
      else tree

let balR tree = match tree with
  Leaf -> tree |
  Node (h, a, left, Leaf) -> tree |
  Node (h0, a, t1, Node (h1, c, t2, t3)) ->
    if (h1 = ht t1 + 2) 
      then if ht t2 <= ht t3 
        then Node (h0 - 1, c, Node (ht t2 + 1, a, t1, t2), t3)
        else match t2 with Leaf -> tree |
          Node (h2, b, t21, t22) -> 
            Node (h0 - 1, b, Node (ht t1 + 1, a, t1, t21), Node (ht t3 + 1, c, t22, t3))
    else tree

let rec insert x tree = match tree with
  Leaf -> Node (1, x, Leaf, Leaf) |
  Node (h, a, left, right) -> 
    if x < a then let new_left = insert x left in 
      balL (Node (max (ht new_left) (ht right) + 1, a, new_left, right))
  else if x > a then let new_right = insert x right in 
      balR (Node (max (ht left) (ht new_right) + 1, a, left, new_right))
  else tree

