#include <parser.h>

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    Token curr_token = tokenizer->GetToken();

    if (tokenizer->IsEnd()) {
        throw SyntaxError{"Invalid input"};
    }
    if (std::holds_alternative<BooleanToken>(curr_token)) {
        tokenizer->Next();
        return std::make_shared<Boolean>(std::get<BooleanToken>(curr_token).state);
    }
    if (std::holds_alternative<QuoteToken>(curr_token)) {
        tokenizer->Next();
        if (tokenizer->IsEnd()) {
            throw SyntaxError{"Invalid Usage of Quote"};
        }
        return std::make_shared<Cell>(std::make_shared<Symbol>("quote"), Read(tokenizer));
    }
    if (std::holds_alternative<ConstantToken>(curr_token)) {
        tokenizer->Next();
        return std::make_shared<Number>(std::get<ConstantToken>(curr_token).value);
    }

    if (std::holds_alternative<SymbolToken>(curr_token)) {
        tokenizer->Next();
        return std::make_shared<Symbol>(std::get<SymbolToken>(curr_token).name);
    }

    if (std::holds_alternative<DotToken>(curr_token)) {
        tokenizer->Next();
        return std::make_shared<Symbol>(".");
    }

    if (std::holds_alternative<BracketToken>(curr_token) &&
        std::get<BracketToken>(curr_token) == BracketToken::OPEN) {
        tokenizer->Next();
        if (std::holds_alternative<BracketToken>(tokenizer->GetToken()) &&
            std::get<BracketToken>(tokenizer->GetToken()) == BracketToken::CLOSE) {
            tokenizer->Next();
            return nullptr;
        }
        if (std::holds_alternative<SymbolToken>(tokenizer->GetToken()) &&
            std::get<SymbolToken>(tokenizer->GetToken()).name == "quote") {
            tokenizer->Next();
            if (tokenizer->IsEnd()) {
                throw SyntaxError{"Invalid Usage of Quote"};
            }
            return std::make_shared<Cell>(std::make_shared<Symbol>("quote"), Read(tokenizer));
        }
        return ReadList(tokenizer);
    }
    throw SyntaxError{"Invalid input"};
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    auto root = std::make_shared<Cell>();
    auto curr_node = root;

    Token curr_token = tokenizer->GetToken();

    while (true) {

        if (tokenizer->IsEnd()) {
            throw SyntaxError{"Invalid input"};
        }

        if (std::holds_alternative<BracketToken>(curr_token) &&
            std::get<BracketToken>(tokenizer->GetToken()) == BracketToken::CLOSE) {
            break;
        }

        auto object = Read(tokenizer);

        if (Is<Symbol>(object) && As<Symbol>(object)->GetName() == ".") {
            if (!curr_node->GetFirst()) {
                throw SyntaxError("Invalid Pair");
            }
            curr_node->SetSecond(Read(tokenizer));
            curr_token = tokenizer->GetToken();
            continue;
        }

        if (!curr_node->GetFirst()) {
            curr_node->SetFirst(object);
        } else {
            if (curr_node->GetSecond()) {
                throw SyntaxError{"Invalid List"};
            }
            curr_node->SetSecond(std::make_shared<Cell>());
            curr_node = As<Cell>(curr_node->GetSecond());
            curr_node->SetFirst(object);
        }
        curr_token = tokenizer->GetToken();
    }
    tokenizer->Next();
    return root;
}