open Functional.Rbt

let exmpl = Node(Empty, (5, Red), Empty)
let () = 
    let c = get_color exmpl in 
    Printf.printf "Color: %s\n" (color_to_string c)
