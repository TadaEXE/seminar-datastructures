open Avl_functional.Datastructure
open Avl_functional.Input
open Benchmark

let rec balanced_tree a depth =
  if depth <= 0 then Leaf 
  else Node (BAL, a, (balanced_tree (a / 2) (depth - 1)), (balanced_tree (a + (a / 2)) (depth - 1)))

let rec spaced_tree_const min step depth = match depth with 
  0 -> (Leaf, min) |
  1 -> (Node (BAL, min, Leaf, Leaf), min + step) |
  depth ->  let (left, left_max) = spaced_tree_const min step (depth - 1) in
    let (right, right_max) = spaced_tree_const (left_max + step) step (depth - 2) in
    (Node (LH, left_max, left, right), right_max)

let spaced_tree step depth = let (tree, max) = spaced_tree_const step step depth in tree

let rec unbalanced_tree a depth = 
  if depth <= 0 then Leaf 
    else Node (LH, a, (unbalanced_tree (a / 2) (depth - 1)), (unbalanced_tree (a + (a / 2)) (depth - 2)))

let rec execute_actions act tree = match act with
  [] -> tree |
  Insert x :: act -> execute_actions act (insert x tree) |
  Delete x :: act -> execute_actions act (delete x tree) |
  Find x :: act -> let _ = find x tree in execute_actions act tree

let rec benchmark tree ic n =  
  if n <= 0 then []
  else let actions = try read_segment ic with End_of_file -> [] in
    if actions == [] then []
    else let v = latency1 100L (execute_actions actions) tree in
    let wc = (match  v with (s, t1 :: ts) :: vs -> t1.cutime | v -> 0.0) in 
    wc :: benchmark (execute_actions actions tree) ic (n - 1) 
(*
let benchmark_balanced = let tree1 = balanced_tree 1073741824 25 in
    let _ = print_endline ("Constructed tree of size " ^ string_of_int (size tree1)) in
    let ic = open_in "./data/benchmark_insert.txt" in
    let _ = benchmark tree1 ic 100 in
    ()
*)
let benchmark_unbalanced = 
    let tree2 = spaced_tree 64 35 in
    let _ = print_endline ("Constructed tree of size " ^ string_of_int (size tree2)) in
    let ic = open_in "./data/benchmark_insert.txt" in
    let _ = benchmark tree2 ic 100 in
    ()