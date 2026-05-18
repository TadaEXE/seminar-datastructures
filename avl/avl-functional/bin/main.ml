open Avl_functional.Datastructure

(* Fisher-Yates shuffle — mutates the array in place *)
let shuffle rng arr =
  let n = Array.length arr in
  for i = n - 1 downto 1 do
    let j = Random.State.int rng (i + 1) in
    let tmp = arr.(i) in
    arr.(i) <- arr.(j);
    arr.(j) <- tmp
  done

(* Measure insert → lookup → delete for a key array.
   Returns (insert_ms, lookup_ms, delete_ms). *)
let bench keys =
  let t0 = Sys.time () in
  let tree = Array.fold_left (fun t k -> insert k t) Leaf keys in
  let t1 = Sys.time () in
  Array.iter (fun k -> ignore (find k tree)) keys;
  let t2 = Sys.time () in
  let _empty = Array.fold_left (fun t k -> delete k t) tree keys in
  let t3 = Sys.time () in
  ((t1 -. t0) *. 1000.0,
   (t2 -. t1) *. 1000.0,
   (t3 -. t2) *. 1000.0)

let run_benchmarks () =
  let rng = Random.State.make [| 42 |] in
  Printf.printf "\n=== Benchmarks (OCaml) ===\n";
  List.iter (fun n ->
    let sorted = Array.init n (fun i -> i) in
    let random = Array.copy sorted in
    shuffle rng random;
    let (ri, rl, rd) = bench random in
    let (si, sl, sd) = bench sorted in
    Printf.printf
      "  %-6s  n=%8d  ins=%8.2f ms  lkp=%8.2f ms  del=%8.2f ms\n"
      "random" n ri rl rd;
    Printf.printf
      "  %-6s  n=%8d  ins=%8.2f ms  lkp=%8.2f ms  del=%8.2f ms\n"
      "sorted" n si sl sd
  ) [10_000; 100_000; 1_000_000]

let () = run_benchmarks ()
