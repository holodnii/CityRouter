#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json{

	class Node;
	using Dict = std::map<std::string, Node>;
	using Array = std::vector<Node>;

	class ParsingError : public std::runtime_error {
	public:
		using runtime_error::runtime_error;
	};

	class Node final : public std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>{
	public:
		using variant::variant;
		using Value = variant;

		Node(const json::Node::Value& value)
			: Value(value)
		{}

		bool IsInt() const{
			return std::holds_alternative<int>(*this);
		}
		bool IsPureDouble() const{
			return std::holds_alternative<double>(*this);
		}
		bool IsDouble() const{
			return IsInt() || IsPureDouble();
		}
		bool IsBool() const{
			return std::holds_alternative<bool>(*this);
		}
		bool IsNull() const{
			return std::holds_alternative<std::nullptr_t>(*this);
		}
		bool IsArray() const{
			return std::holds_alternative<Array>(*this);
		}
		bool IsString() const{
			return std::holds_alternative<std::string>(*this);
		}
		bool IsMap() const{
			return std::holds_alternative<Dict>(*this);
		}

		int AsInt() const{
			using namespace std::literals;
			if (!IsInt()){
				throw std::logic_error("not an int"s);
			}
			return std::get<int>(*this);
		}

		double AsDouble() const{
			using namespace std::literals;
			if (!IsDouble()){
				throw std::logic_error("Not a double"s);
			}

			return IsPureDouble() ? std::get<double>(*this) : AsInt();
		}

		bool AsBool() const{
			using namespace std::literals;
			if (!IsBool()){
				throw std::logic_error("Not a bool"s);
			}

			return std::get<bool>(*this);
		}

		const Array& AsArray() const{
			using namespace std::literals;
			if (!IsArray()){
				throw std::logic_error("Not an array"s);
			}

			return std::get<Array>(*this);
		}

		const std::string& AsString() const{
			using namespace std::literals;
			if (!IsString()){
				throw std::logic_error("Not a string"s);
			}

			return std::get<std::string>(*this);
		}

		const Dict& AsMap() const{
			using namespace std::literals;
			if (!IsMap()){
				throw std::logic_error("Not a dict"s);
			}

			return std::get<Dict>(*this);
		}

		const Value& GetValue() const{
			return *this;
		}
	};

	inline bool operator==(const Node& lhs, const Node& rhs){
		return lhs.GetValue() == rhs.GetValue();
	}

	inline bool operator!=(const Node& lhs, const Node& rhs){
		return lhs.GetValue() != rhs.GetValue();
	}

	class Document {
	public:
		explicit Document(Node root) : root_(std::move(root)){}

		const Node& GetRoot() const{
			return root_;
		}

	private:
		Node root_;
	};

	inline bool operator==(const Document& lhs, const Document& rhs){
		return lhs.GetRoot() == rhs.GetRoot();
	}

	inline bool operator!=(const Document& lhs, const Document& rhs){
		return !(lhs == rhs);
	}

    struct NodeGetter{
        Node operator() (std::nullptr_t) const{
            return Node{};
        }
        Node operator() (Array value) const{
            return Node(std::move(value));
        }
        Node operator() (Dict value) const{
            return Node(std::move(value));
        }
        Node operator() (bool value) const{
            return Node(value);
        }
        Node operator() (int value) const{
            return Node(value);
        }
        Node operator() (double value) const{
            return Node(value);
        }
        Node operator() (std::string value) const{
            return Node(std::move(value));
        }
    };

	Document Load(std::istream& input);
	void Print(const Document& doc, std::ostream& output);
}
