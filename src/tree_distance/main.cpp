#include <iostream>
#include <string>

#include "node.h"
#include "apted_tree_index.h"
#include "string_label.h"
#include "bracket_notation_parser.h"
#include "unit_cost_model.h"
#include "ted_algorithm.h"
#include "tree_indexer.h"

int main(int argc, char** argv) {
    // Type aliases.
    using Label = label::StringLabel;
    using CostModel = cost_model::UnitCostModelLD<Label>;
    using LabelDictionary = label::LabelDictionary<Label>;

    parser::BracketNotationParser bnp;

    // Verify parameters.
    if (argc != 3) {
        std::cerr << "Incorrect number of parameters." << std::endl;
        return -1;
    }

    // Read src and dst trees from files
    std::ifstream tree_file(argv[1]);
    std::string tree_string;
    std::getline(tree_file, tree_string);
    if (!bnp.validate_input(tree_string)) {
        std::cerr << "Incorrect format of source tree. Is the number of opening and closing brackets equal?"
                  << std::endl;
        return -1;
    }
    const node::Node<Label> t1 = bnp.parse_single(tree_string);
    tree_file.close();

    tree_file = std::ifstream(argv[2]);
    std::getline(tree_file, tree_string);
    if (!bnp.validate_input(tree_string)) {
        std::cerr << "Incorrect format of destnation tree. Is the number of opening and closing brackets equal?"
                  << std::endl;
        return -1;
    }
    const node::Node<Label> t2 = bnp.parse_single(tree_string);
    tree_file.close();

    LabelDictionary ld;

    // Initialise cost model.
    CostModel ucm(ld);

    ted::TEDAlgorithm<CostModel, node::TreeIndexAll>* ted_algorithm;

    // Initialise ZS algorithm.
    ted::APTEDTreeIndex<CostModel, node::TreeIndexAll> apted_algorithm(ucm);

    // Initialise two tree indexes.
    // Use TreeIndexAll that is a superset of all algorithms' indexes.
    node::TreeIndexAll ti1;
    node::TreeIndexAll ti2;
    // Index input trees.
    node::index_tree(ti1, t1, ld, ucm);
    node::index_tree(ti2, t2, ld, ucm);

    ted_algorithm = &apted_algorithm;
    // Execute the algorithm.
    double computed_results = ted_algorithm->ted(ti1, ti2);
    std::cout << "TED result: " << computed_results << std::endl;

    return 0;
}