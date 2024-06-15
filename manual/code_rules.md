# Code rules {#code_rules}
### Common approach.
Try to minimize the number of idioms and terms inside a project. If one of the terms changes - do not hesitate to refactor the whole project.

### Function arguments and return types.
If the function is not time critical - pass parameters and return types by value, to improve code redability and get rid of extra code regarding object lifetime.
i.e. const QString& -> QString (since QString use COW (copy-on-write))
Thus, using less modifiers results in improved readability.

If we need to pass an object as a read-only parameter, and its copying is time/memory consuming - use const T&
  
### struct vs class.
if all fields are independend and we dont do manual resource managment, use struct.
in other cases - class
  
### Resources.
No global data or singletons.
Try to use static fields.
  
### Object interconnection.
Use signal/slot connections, including functions, returning value. 

### Qt parent
Use parent only for GUI widgets. do not use it for object lifetime managment.



  
