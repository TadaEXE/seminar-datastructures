open OUnit2
open Avl_functional.Input
open Avl_functional.Datastructure

let generate_actions path = read_segment (open_in path)

let execute_single_action act tree i = match act with
  Insert x -> let new_tree = insert x tree in
    let x_str = string_of_int x in let i_str = string_of_int i in
    let _ = assert_bool (i_str ^ ": Insert " ^ x_str ^ " tree invariant") (search_tree_inv new_tree) in
    let _ = assert_bool (i_str ^ ": Insert " ^ x_str ^ " avl invariant") (avl_inv new_tree) in
    let _ = assert_bool (i_str ^ ": Insert " ^ x_str ^ " find") (find x new_tree) in
    let _ = print_endline (i_str ^ ": Insert " ^ x_str) in
    new_tree |
  Delete x -> let new_tree = delete x tree in
  let x_str = string_of_int x in let i_str = string_of_int i in
    let _ = assert_bool (i_str ^ ": Delete " ^ x_str ^ " tree invariant") (search_tree_inv new_tree) in
    let _ = assert_bool (i_str ^ ": Delete " ^ x_str ^ " avl invariant") (avl_inv new_tree) in
    let _ = assert_bool (i_str ^ ": Delete " ^ x_str ^ " find") (not (find x new_tree)) in
    let _ = print_endline (i_str ^ ": Delete " ^ x_str) in
    new_tree |
  Find x ->     
    let x_str = string_of_int x in let i_str = string_of_int i in
    let _ = assert_bool (i_str ^ ": Find " ^ x_str) (find x tree) in
    let _ = print_endline (i_str ^ ": Find " ^ x_str) in
    tree

let debug_single_action act tree i = match act with 
    Insert x -> insert x tree |
    Delete x -> let new_tree = delete x tree in
      let _ = if x == 6 then
         print_endline ("Sizes: " ^ (string_of_int (size tree)) ^ ", " ^ (string_of_int (size new_tree)))
      else () in
      let _ = if i == 65 then print_avl tree in
      let _= if i == 65 then print_avl new_tree in 
      new_tree |
    Find x -> tree

let rec debug_actions actions tree i = match actions with 
    [] -> () |
    act :: actions -> let new_tree = debug_single_action act tree i in
      debug_actions actions new_tree (i + 1)

let rec execute_actions actions tree i = match actions with 
    [] -> () |
    act :: actions -> let new_tree = execute_single_action act tree i in
      execute_actions actions new_tree (i + 1)

let mini_tree = Node (LH, 6, Node (BAL, 5, Leaf, Leaf), Leaf)

let tests = "test suite for sum" >::: [
  "Random test" >:: (fun _ -> execute_actions (generate_actions "./data/test_random.txt") Leaf 0);
]

let _ = run_test_tt_main tests
