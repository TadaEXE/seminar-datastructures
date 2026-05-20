open Avl_functional.Datastructure
open Avl_functional.Input
open Benchmark

let rec balanced_tree a depth =
  if depth <= 0 then Leaf 
  else Node (BAL, a, (balanced_tree (a / 2) (depth - 1)), (balanced_tree (a + (a / 2)) (depth - 1)))

let rec execute_actions act tree = match act with
  [] -> tree |
  Insert x :: act -> execute_actions act (insert x tree) |
  Delete x :: act -> execute_actions act (delete x tree) |
  Find x :: act -> let _ = find x tree in execute_actions act tree

let rec benchmark tree ic n =  
  if n <= 0 then () 
  else let actions = try read_segment ic with End_of_file -> [] in
    if actions == [] then ()
    else let _ = latency1 500L (execute_actions actions) tree in
    benchmark (execute_actions actions tree) ic (n - 1) 
    
let benchmark_all = let tree = balanced_tree 1073741824 25 in
    let _ = print_endline "Construction Done" in
    let ic = open_in "./data/benchmark_10000.txt" in
    benchmark tree ic 100
