type bal = LH | BAL | RH
type 'a avl = Leaf | Node of (bal * 'a * 'a avl * 'a avl)

let rec ht tree = 
  match tree with 
  Leaf -> 0 |
  Node (BAL, _, t1, t2) -> ht t1 + 1 |
  Node (LH, _, t1, t2) -> ht t1 + 1 |
  Node (RH, _, t1, t2) -> ht t2 + 1

let rec avl_inv tree =
  match tree with
  Leaf -> true |
  Node (LH, _, left, right) -> ht left = ht right + 1 && avl_inv left && avl_inv right |
  Node (BAL, _, left, right) -> ht left = ht right && avl_inv left && avl_inv right |
  Node (RH, _, left, right) -> ht left + 1 = ht right && avl_inv left && avl_inv right


let rec print_avl_rec tree = match tree with 
  Leaf -> "<>" |
  Node (BAL, a, left, right) -> 
            "(" ^ print_avl_rec left ^ ", =, " ^
            string_of_int a ^ "," ^ print_avl_rec right ^ ")" |
  Node (LH, a, left, right) -> 
            "(" ^ print_avl_rec left ^ ", <-, " ^
            string_of_int a ^ "," ^ print_avl_rec right ^ ")" |
  Node (RH, a, left, right) -> 
            "(" ^ print_avl_rec left ^ ", ->, " ^
            string_of_int a ^ "," ^ print_avl_rec right ^ ")"

let print_avl tree = print_newline (print_string (print_avl_rec tree))


let rec find x tree =
    match x, tree with
    a, Leaf -> false |
    a, Node (h, b, left, right) -> a == b || find x left || find x right


let is_bal tree = match tree with
  Leaf -> true |
  Node(BAL, _, _, _) -> true |
  tree -> false

let incr old changed = (old == Leaf && changed != Leaf || is_bal old && not (is_bal changed))

let rot2 t1 a t2 c t3 = match t2 with
  Node (BAL, b, t21, t22) -> Node (BAL, b, Node(BAL, a, t1, t21), Node (BAL, c, t22, t3)) |
  Node (LH, b, t21, t22) -> Node (BAL, b, Node(BAL, a, t1, t21), Node (RH, c, t22, t3)) |
  Node (RH, b, t21, t22) -> Node (BAL, b, Node(LH, a, t1, t21), Node (BAL, c, t22, t3)) |
  (* This case should be impoassible I think *)
  Leaf -> Leaf 

let balL tree = match tree with
  Leaf -> tree |
  Node (b, a, Leaf, right) -> tree |
  Node (LH, c, Node (RH, a, t1, t2), t3) -> rot2 t1 a t2 c t3 |
  Node (LH, c, Node (LH, a, t1, t2), t3) -> Node (BAL, a, t1, Node (BAL, c, t2, t3)) |
  Node (LH, c, Node (BAL, a, t1, t2), t3) -> Node (RH, a, t1, Node (LH, c, t2, t3)) |
  Node (BAL, c, left, right) -> Node (LH, c, left, right) |
  Node (RH, c, left, right) -> Node (BAL, c, left, right)


let balR tree = match tree with
  Leaf -> tree |
  Node (b, a, left, Leaf) -> tree |
  Node (RH, a, t1, Node (LH, c, t2, t3)) -> rot2 t1 a t2 c t3 |
  Node (RH, a, t1, Node (RH, c, t2, t3)) -> Node (BAL, a, Node (BAL, c, t1, t2), t3) |
  Node (RH, a, t1, Node (BAL, c, t2, t3)) -> Node (RH, a, Node (LH, c, t1, t2), t3) |
  Node (BAL, a, t1, t2) -> Node (RH, a, t1, t2) |
  Node (LH, a, t1, t2) -> Node (BAL, a, t1, t2)
  


let rec insert x tree = match tree with
  Leaf -> Node (BAL, x, Leaf, Leaf) |
  Node (bl, a, left, right) -> 
    if x < a then let new_left = insert x left in 
      if incr left new_left 
        then balL (Node (bl, a, new_left, right))
        else Node (bl, a, new_left, right)
    else if x > a then let new_right = insert x right in 
      if incr right new_right 
        then balR (Node (bl, a, left, new_right))
    else Node (bl, a, left, new_right)
  else tree

let decr old changed = match old with 
  Leaf -> false |
  old -> incr changed old

let rec split_max tree = match tree with
  Leaf -> (Leaf, 0) |
  Node (bl, a, left, Leaf) -> (left, a) |
  Node (bl, a, left, right) -> 
    let (r1, a1) = split_max right in 
    if incr r1 right then (balL (Node (bl, a, left, r1)), a1)
    else (Node (bl, a, left, r1), a1)

let rec delete x tree = match tree with
  Leaf -> Leaf |
  Node (bl, a, left, right) -> 
    if x < a then let new_left = delete x left in 
      if decr left new_left then balR (Node (bl, a, new_left, right))
      else Node (bl, a, new_left, right)
    else if x > a then let new_right = delete x right in 
      if decr right new_right then balL (Node (bl, a, left, new_right))
      else Node (bl, a, left, new_right)
    else if left == Leaf then right
    else let (l1, a1) = split_max left in 
      if incr l1 left 
        then balR (Node (bl, a1, l1, right))
        else Node (bl, a1, l1, right)

