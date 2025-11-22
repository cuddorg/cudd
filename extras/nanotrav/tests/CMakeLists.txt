add_test(
  NAME test_C17
  COMMAND $<TARGET_FILE:nanotrav> -p 1 -cover ${CMAKE_CURRENT_SOURCE_DIR}/data/C17.blif
)

add_test(
  NAME test_C880
  COMMAND $<TARGET_FILE:nanotrav> -p 1 -ordering dfs -autodyn -automethod sifting -reordering sifting -drop ${CMAKE_CURRENT_SOURCE_DIR}/data/C880.blif
)

add_test(
  NAME test_s27
  COMMAND $<TARGET_FILE:nanotrav> -p 1 -ordering hw -reordering annealing -trav ${CMAKE_CURRENT_SOURCE_DIR}/data/s27.blif
)

add_test(
  NAME test_s27b
  COMMAND $<TARGET_FILE:nanotrav> -p 1 -ordering dfs -reordering win4 -verify ${CMAKE_CURRENT_SOURCE_DIR}/data/s27.blif ${CMAKE_CURRENT_SOURCE_DIR}/data/s27b.blif
)

add_test(
  NAME test_s27c
  COMMAND $<TARGET_FILE:nanotrav> -p 1 -trav -image depend -depend ${CMAKE_CURRENT_SOURCE_DIR}/data/s27c.blif
)

add_test(
  NAME test_mult32a
  COMMAND $<TARGET_FILE:nanotrav> -p 1 -autodyn -reordering sifting -trav ${CMAKE_CURRENT_SOURCE_DIR}/data/mult32a.blif
)

add_test(
  NAME test_s382
  COMMAND $<TARGET_FILE:nanotrav> -p 1 -trav -image part -autodyn -automethod sifting -drop -scc -shortpaths bellman ${CMAKE_CURRENT_SOURCE_DIR}/data/s382.blif
)

add_test(
  NAME test_s641
  COMMAND $<TARGET_FILE:nanotrav> -p 1 -trav -autodyn -automethod group -drop -clauses -density -decomp -zdd ${CMAKE_CURRENT_SOURCE_DIR}/data/s641.blif
)

add_test(
  NAME test_closest
  COMMAND $<TARGET_FILE:nanotrav> -p 1 -reordering genetic -drop -closest ${CMAKE_CURRENT_SOURCE_DIR}/data/closest.blif
)

add_test(
  NAME test_adj49
  COMMAND $<TARGET_FILE:nanotrav> -p 1 -ordering dfs -reordering cogroup -drop -char2vect -cofest ${CMAKE_CURRENT_SOURCE_DIR}/data/adj49.blif
)

add_test(
  NAME test_ham01
  COMMAND $<TARGET_FILE:nanotrav> -p 1 -reordering linear ${CMAKE_CURRENT_SOURCE_DIR}/data/ham01.blif
)

add_test(
  NAME test_miniFirst
  COMMAND $<TARGET_FILE:nanotrav> -p 1 -second ${CMAKE_CURRENT_SOURCE_DIR}/data/miniSecond.blif ${CMAKE_CURRENT_SOURCE_DIR}/data/miniFirst.blif
)

add_test(
  NAME test_rcn25
  COMMAND $<TARGET_FILE:nanotrav>  -p 1 -envelope ${CMAKE_CURRENT_SOURCE_DIR}/data/rcn25.blif
)
