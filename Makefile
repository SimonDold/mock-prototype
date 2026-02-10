main: *.cc *.h
	g++ -std=c++20 main.cc state_id.cc operator_id.cc evaluators/*.cc search_algorithms/*.cc open_lists/*.cc -o main
