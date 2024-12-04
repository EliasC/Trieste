#pragma once

#include "token.h"
#include "ast.h"

namespace trieste {

    namespace unit_test{


    inline bool type_equals (Node n, Node m){
        return n->type() == m->type();
    }

    inline bool value_equals (Node n, Node m){
        return n->location().view() == m->location().view();
    }

    inline bool node_equals (Node n, Node m)
    {
        NodeIt n_it, m_it;
        if (type_equals(n,m) && value_equals(n,m)){
            n_it = n->begin();
            m_it = m->begin();
            while (!(n_it == n->end()) || !(m_it == m->end())){
                if(!node_equals(*n_it,*m_it)) return false;
                n_it++; m_it++;
            }
        }
        return true;
    }

    struct Assertion {
        Node before_;
        Node expected_;
        std::function<bool(Node,Node)> assertion;
    };

    struct Test {
        std::string desc_;
        std::vector<Assertion> assertions;
        std::function<Node(Node)> rewrite_;


        Test(std::string desc, std::function<Node(Node)> rewrite)
          : desc_(desc), rewrite_(rewrite) {}

        bool run() {
            bool result = true;
            for (auto assertion : assertions) {
                logging::Output() << "Testing " << desc_ << std::endl
                                  << "before:\n" << assertion.before_ << std::endl;

                auto actual = rewrite_(assertion.before_);
                bool ok = assertion.assertion(actual,assertion.expected_);
                result = ok && result;

                // TODO: Only print if the test failed
                logging::Output() << "expected: " << assertion.expected_ << std::endl
                                  << "actual: " << actual << std::endl
                                  << (ok ? "test passed" : "test failed") << std::endl;
            }
            return result;
        }

        void isEqual(Node before, Node expected){
            assertions.push_back(
                {before,
                 expected,
                 [](Node res, Node exp)
                    {
                    return node_equals(res,exp);
                    }
                }
            );
        }
    };

    }
}
