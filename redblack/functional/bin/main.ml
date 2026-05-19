open Functional.Rbt

let exmpl = insert 12 (insert 1 (insert 10 (insert 6 (insert 3 (Node(Empty, (5, Red), Empty))))))
let exmpl_str = insert "A" (insert "Aello" (insert "Hello" (Node(Empty, ("World", Black), Empty))))
let () = 
    (*Printf.printf "Tree: %s\n" (tree_to_string exmpl)*)
    tree_to_ascii (string_of_int) exmpl "";
    Printf.printf "<------------------------------------------>\n";
    tree_to_ascii (fun x -> x) exmpl_str ""
