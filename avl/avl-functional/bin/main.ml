open Avl_functional.Datastructure
open Avl_functional.Input
open Benchmark

let rec pow2 n = if n == 0 then 1 else 2 * pow2 (n - 1)

let rec balanced_tree min step depth = 
  if depth <= 0 then Leaf 
  else let a = min + (pow2 (depth - 1)) * step in 
    Node (BAL, a, (balanced_tree min step (depth - 1)), (balanced_tree a step (depth - 1)))

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

let rec length lst = match lst with [] -> 0 | _ :: lst -> 1 + length lst

let rec benchmark tree ic n =  
  if n <= 0 then let _ = close_in ic in []
  else let actions = try read_segment ic with End_of_file -> [] in
    if actions == [] then []
    else let v = latency1 100L (execute_actions actions) tree in
    let wc = (match  v with (s, t1 :: ts) :: vs -> t1 | v -> (make 0L)) in 
    wc :: benchmark (execute_actions actions tree) ic (n - 1)

let rec print_result_rec res oc = match res with 
    t :: [] -> let _ = print_endline (string_of_float t.wall) in
      let _ = Printf.fprintf oc "(%f, %f)" t.wall t.minor_words in () |
    t :: res -> let _ = print_endline (string_of_float t.wall) in
      let _ = Printf.fprintf oc "(%f, %f), " t.wall t.minor_words in
      print_result_rec res oc |
    [] -> ()

let print_result name res oc = 
  let _ = Printf.fprintf oc "\"%s\":[" name in 
  let _= print_result_rec res oc in
  let _=  Printf.fprintf oc "],\n" in 
  flush oc

let benchmark_balanced n path oc = 
  let tree1 = balanced_tree 0 64 n in
  let _ = print_endline ("Constructed tree of size " ^ string_of_int (size tree1)) in
  let ic = open_in path in 
  let res = benchmark tree1 ic 100 in
  print_result ("Balanced_" ^ string_of_int n) res oc
  


let benchmark_unbalanced n path oc =  
    let tree2 = spaced_tree 64 n in
    let _ = print_endline ("Constructed tree of size " ^ string_of_int (size tree2)) in
    let ic = open_in path in 
    let res = benchmark tree2 ic 100 in
    print_result ("Unbalanced_" ^ string_of_int n) res oc

let balanced_benchmarks = [|
(15, "data/range_32767.txt");
(16, "data/range_65535.txt");
(17, "data/range_131071.txt");
(18, "data/range_262143.txt");
(19, "data/range_524287.txt");
(20, "data/range_1048575.txt");
(21, "data/range_2097151.txt");
(22, "data/range_4194303.txt");
(23, "data/range_8388607.txt");
(24, "data/range_16777215.txt");
(25, "data/range_33554431.txt");
|]

let unbalanced_benchmarks = [|
  (21, "./data/benchmark_15.txt"); 
  (28, "./data/benchmark_20.txt");
  (34, "./data/benchmark_24.txt")|]

(*let benchmark_all = let oc = open_out "./data/results_long1.txt" in
  let _= Array.iter (fun (n, path) -> benchmark_balanced n path oc)  balanced_benchmarks in
  Array.iter (fun (n, path) -> benchmark_unbalanced n path oc) unbalanced_benchmarks*)


let benchmark_bal = let oc = open_out "./data/results_balanced.txt" in
  Array.iter (fun (n, path) -> benchmark_balanced n path oc)  balanced_benchmarks

(* In this case, the maximum is the most shallow node *)
let rec max tree = match tree with
  Leaf -> 0 |
  Node (_, x, _, Leaf) -> x |
  Node (_, _, _, right) -> max right

(* In this case, the minimum is the deepest Node *) 
let rec min tree = match tree with
  Leaf -> 0 |
  Node (_, x, Leaf, _) -> x |
  Node (_, _, left, _) -> min left

let rec benchmark_rep n t x_fun =
  match t with 
  0 -> [] | 
  t ->
    let tree = unbalanced_tree 64 n in
    let x = x_fun tree in 
    let v = latency1 4000000L (insert x) tree in
    let dat = match v with (s, t1 :: ts) :: vs -> t1 | v -> (make 0L) in
    dat :: benchmark_rep n (t - 1) x_fun

let comp_n = [|15;16;17;18;19;20;21;22;23;24;25;26;27;28;29;30;31;32;33;34;35|]

(*
let benchmark_wb = let oc = open_out "./data/results_comp.txt" in
  let _ = print_endline "Best case insert" in
  let i_opt = Array.map (fun n -> (benchmark_rep n 5 (fun tree -> max tree + 1), n)) comp_n in
  let _ = Array.iter (fun (dat, n) -> print_result ("i_opt_" ^ (string_of_int n)) dat oc) i_opt in
  let _ = print_endline "Worst case insert" in
  let i_worst = Array.map (fun n -> (benchmark_rep n 5 (fun tree -> min tree - 1), n)) comp_n in
  let _ = Array.iter (fun (dat, n) -> print_result ("i_worst_" ^ (string_of_int n)) dat oc) i_worst in
  let _ = print_endline "Best case delete" in
  let d_opt = Array.map (fun n -> (benchmark_rep n 5 (fun tree -> min tree), n)) comp_n in
  let _ = Array.iter (fun (dat, n) -> print_result ("d_opt" ^ (string_of_int n)) dat oc) d_opt in
  let _ = print_endline "Worst case delete" in
  let d_worst = Array.map (fun n -> (benchmark_rep n 5 (fun tree -> max tree), n)) comp_n in
  let _ = Array.iter (fun (dat, n) -> print_result ("d_worst" ^ (string_of_int n)) dat oc) d_worst in 
  ()
  *)