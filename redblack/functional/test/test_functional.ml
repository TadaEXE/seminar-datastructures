open Functional.Rbt

let t1 = Node(Node(Empty, (2, Red), Empty), (3, Black), Node(Empty, (4, Red), Empty))
let t2 = Node(Node(Empty, (2, Red), Empty), (3, Black), Empty)
let t3 = Node(Empty, (3, Black), Node(Empty, (4, Red), Empty))
let t4 = Node(Empty, (3, Black), Empty)

let t5 = (insert 1 (insert 200 (insert 100 (insert 2 Empty))))
let t6 = (delete 1 (delete 200 (delete 100 (delete 2 t5))))


let () = 
  assert (lookup t1 2 = Some 2);
  assert (lookup t1 5 = None);
  assert (lookup t2 2 = Some 2);
  assert (lookup t2 5 = None);
  assert (lookup t3 3 = Some 3);
  assert (lookup t3 5 = None);
  assert (lookup t4 3 = Some 3);
  assert (lookup t4 5 = None);

  assert (insert 4 t2 = t1);
  assert (insert 2 t3 = t1);
  assert (insert 2 t4 = t2);
  assert (insert 4 t4 = t3);

  assert (delete 2 t1 = t3);
  assert (delete 4 t1 = t2);
  assert (delete 4 (delete 2 t1) = t4);
  assert (delete 2 t2 = t4);
  assert (delete 4 t3 = t4);

  assert (invrbt t1 = true);
  assert (invrbt t2 = true);
  assert (invrbt t3 = true);
  assert (invrbt t4 = true);
  assert (invrbt t5 = true);
  assert (invrbt t6 = true);

  Printf.printf "Finished tests.\n"