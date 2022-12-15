#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <numeric>
#include <Windows.h>
#include <cmath>
#include <algorithm>
#include <random>

using namespace std;
using std::string;

// Hearts - черви 
// Spades - пики
// Diamonds - буби
// Clubs - крести

int sleep_time = 500;
vector<int> balance = { 200, 200, 200, 200, 200 };
int bank = 0;
vector<int> bets = { 0, 0, 0, 0, 0 };
vector<vector<vector<string>>> player_cards = { {{"", ""}, {"", ""}}, {{"", ""}, {"", ""}}, {{"", ""}, {"", ""}}, {{"", ""}, {"", ""}}, {{"", ""}, {"", ""}} };
vector<string> card_weights = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
vector<string> card_suits = { "h", "s", "d", "c" };
vector<vector<string>> cards_to_use = {};
vector<vector<string>> cards_on_table = { {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""} };
vector<int> fold_players = { 0, 0, 0, 0, 0 };
vector<vector<vector<int>>> player_combinations = {};
vector<int> winners = {};
vector<string> combination_names = {"High card", "Pair", "Two pairs", "Three of a kind", "Straight", "Flush", "Full house", "Four of a kind", "Straight flush"};

int weight_to_int(string weight) {
    vector<string>::iterator w_itr = find(card_weights.begin(), card_weights.end(), weight);
    return distance(card_weights.begin(), w_itr) + 2;
}

vector<int> check_high_card(int player_id) {
    vector<int> combination = {0};
    for (int card = 0; card < 2; card++) {
        if (weight_to_int(player_cards[player_id][card][0]) > combination[0]) {
            combination[0] = weight_to_int(player_cards[player_id][card][0]);
        }
    }
    return combination;
}

vector<int> check_pair(int player_id, vector<int> weights) {
    vector<int> combination = { 0 };
    for (int weight_num = 0; weight_num < 7; weight_num++) {
        if (count(begin(weights), end(weights), weights[weight_num]) == 2) {
            if (weights[weight_num] > combination[0]) {
                combination[0] = weights[weight_num];
            }
        }
    }
    if (combination[0] != 0) {
        return combination;
    }
    else {
        return {};
    }
}

vector<int> check_two_pairs(int player_id, vector<int> weights) {
    vector<int> combination = { 0, 0 };
    for (int weight_num = 0; weight_num < weights.size(); weight_num++) {
        if (count(begin(weights), end(weights), weights[weight_num]) == 2) {
            if (weights[weight_num] > combination[1]) {
                combination[0] = combination[1];
                combination[1] = weights[weight_num];
            }
        }
    }
    if (combination[0] != 0 and combination[1] != 0) {
        return combination;
    }
    else {
        return {};
    }
}

vector<int> check_three_of_a_kind(int player_id, vector<int> weights) {
    vector<int> combination = { 0 };
    for (int weight_num = 0; weight_num < 7; weight_num++) {
        if (count(begin(weights), end(weights), weights[weight_num]) == 3) {
            if (weights[weight_num] > combination[0]) {
                combination[0] = weights[weight_num];
            }
        }
    }
    if (combination[0] != 0) {
        return combination;
    }
    else {
        return {};
    }
}

vector<int> check_straight(int player_id, vector<int> weights) {
    vector<int> combination = { 0 };
    int straight_percent = 1;

    if (weights.size() >= 5) {
        for (int weight_num = 0; weight_num < weights.size() - 1; weight_num++) {
            if (weights[weight_num] == weights[weight_num + 1]) {
                continue;
            }
            if (weights[weight_num] + 1 == weights[weight_num + 1]) {
                straight_percent++;
                combination[0] = weights[weight_num + 1];
            }
            else {
                if (straight_percent >= 5) {
                    continue;
                }
                straight_percent = 1;
            }
        }
    }

    if (straight_percent >= 5) {
        return combination;
    }
    else {
        return {};
    }
}

vector<int> check_flush(int player_id, vector<int> weights) {
    vector<int> combination = { 0 };
    vector<vector<string>> cards = {};
    for (int card = 0; card < 2; card++) {
        cards.push_back(player_cards[player_id][card]);
    }
    for (int card = 0; card < 5; card++) {
        cards.push_back(cards_on_table[card]);
    }
    vector<string> suits = {};
    for (int card = 0; card < 7; card++) {
        suits.push_back(cards[card][1]);
    }

    string flush_suit = "";
    for (int suit_num = 0; suit_num < 7; suit_num++) {
        if (count(begin(suits), end(suits), suits[suit_num]) >= 5) {
            flush_suit = suits[suit_num];
        }
    }
    if (flush_suit != "") {
        combination[0] = weight_to_int(cards[0][0]);
        for (int card = 0; card < 7; card++) {
            if (weight_to_int(cards[card][0]) > combination[0] and cards[card][1] == flush_suit) {
                combination[0] = weight_to_int(cards[card][0]);
            }
        }
        return combination;
    }
    else {
        return {};
    }
}

vector<int> check_full_house(int player_id, vector<int> weights) {
    vector<int> combination = { 0, 0 };
    vector<int> unique_weights = weights;
    unique(unique_weights.begin(), unique_weights.end());
    for (int weight_num = 0; weight_num < unique_weights.size(); weight_num++) {
        if (count(begin(weights), end(weights), unique_weights[weight_num]) == 2) {
            if (unique_weights[weight_num] > combination[1]) {
                combination[1] = unique_weights[weight_num];
            }
        }
        if (count(begin(weights), end(weights), unique_weights[weight_num]) == 3) {
            if (unique_weights[weight_num] > combination[0]) {
                combination[0] = unique_weights[weight_num];
            }
        }
    }
    if (combination[0] != 0 and combination[1] != 0) {
        return combination;
    }
    else {
        return {};
    }
}

vector<int> check_four_of_a_kind(int player_id, vector<int> weights) {
    vector<int> combination = { 0 };
    for (int weight_num = 0; weight_num < 7; weight_num++) {
        if (count(begin(weights), end(weights), weights[weight_num]) == 4) {
            if (weights[weight_num] > combination[0]) {
                combination[0] = weights[weight_num];
            }
        }
    }
    if (combination[0] != 0) {
        return combination;
    }
    else {
        return {};
    }
}

vector<int> check_straight_flush(int player_id, vector<int> weights) {
    vector<int> combination = { 0 };
    vector<vector<string>> cards = {};

    for (int card = 0; card < 2; card++) {
        cards.push_back(player_cards[player_id][card]);
    }
    for (int card = 0; card < 5; card++) {
        cards.push_back(cards_on_table[card]);
    }

    vector<string> suits = {};
    for (int card = 0; card < 7; card++) {
        suits.push_back(cards[card][1]);
    }
    string flush_suit = "";
    for (int suit_num = 0; suit_num < 7; suit_num++) {
        if (count(begin(suits), end(suits), suits[suit_num]) >= 5) {
            flush_suit = suits[suit_num];
        }
    }
    if (flush_suit != "") {
        vector<int> this_suit_weights = {};
        for (int card = 0; card < 7; card++) {
            if (cards[card][1] == flush_suit) {
                this_suit_weights.push_back(weight_to_int(cards[card][0]));
            }
        }
        sort(this_suit_weights.begin(), this_suit_weights.end());
        if (check_straight(player_id, this_suit_weights).empty() == 0) {
            return check_straight(player_id, this_suit_weights);
        }
        else {
            return {};
        }
    }
    else {
        return {};
    }
}

void check_combinations(int player_id) {
    vector<int> weights = {};

    for (int card = 0; card < 2; card++) {
        weights.push_back(weight_to_int(player_cards[player_id][card][0]));
    }
    for (int card = 0; card < 5; card++) {
        weights.push_back(weight_to_int(cards_on_table[card][0]));
    }
    sort(weights.begin(), weights.end());

    player_combinations[player_id].push_back(check_high_card(player_id));
    player_combinations[player_id].push_back(check_pair(player_id, weights));
    player_combinations[player_id].push_back(check_two_pairs(player_id, weights));
    player_combinations[player_id].push_back(check_three_of_a_kind(player_id, weights));
    player_combinations[player_id].push_back(check_straight(player_id, weights));
    player_combinations[player_id].push_back(check_flush(player_id, weights));
    player_combinations[player_id].push_back(check_full_house(player_id, weights));
    player_combinations[player_id].push_back(check_four_of_a_kind(player_id, weights));
    player_combinations[player_id].push_back(check_straight_flush(player_id, weights));
}

void check_pre_winner() {
    if (accumulate(fold_players.begin(), fold_players.end(), 0) == 4) {
        for (int player_number = 0; player_number < 5; player_number++) {
            if (fold_players[player_number] == 0) {
                winners.push_back(player_number);
            }
        }
    }

}

int calculate_bet(int player_id, bool is_first) {
    int coolness_factor = 0;
    // same suits
    if (player_cards[player_id][0][1] == player_cards[player_id][1][1]) {
        coolness_factor += 3;
    }
    // pair
    if (player_cards[player_id][0][0] == player_cards[player_id][1][0]) {
        coolness_factor += 4;
    }
    // straight draw (hands)
    int w1, w2;
    vector<string>::iterator w1_itr = find(card_weights.begin(), card_weights.end(), player_cards[player_id][0][0]);
    w1 = distance(card_weights.begin(), w1_itr);
    vector<string>::iterator w2_itr = find(card_weights.begin(), card_weights.end(), player_cards[player_id][1][0]);
    w2 = distance(card_weights.begin(), w2_itr);
    if (abs(w1 - w2) > 0 and abs(w1 - w2) <= 4) {
        coolness_factor += 1;
    }
    // higher card
    for (int i = 0; i < 2; i++) {
        if (player_cards[player_id][i][0] == "10" or player_cards[player_id][i][0] == "J") {
            coolness_factor += 1;
        }
        if (player_cards[player_id][i][0] == "Q" or player_cards[player_id][i][0] == "K") {
            coolness_factor += 2;
        }
        if (player_cards[player_id][i][0] == "A") {
            coolness_factor += 3;
        }
    }
    // 6, 10
    if ((player_cards[player_id][0][0] == "6" and player_cards[player_id][1][0] == "10") or (player_cards[player_id][0][0] == "6" and player_cards[player_id][1][0] == "10")) {
        coolness_factor = 8;
    }

    // count bet
    int check_or_raise = rand() % 10 + 1;
    int bet_i_want = (balance[player_id] / check_or_raise * coolness_factor / 10);
    while (bet_i_want % 5 != 0) {
        bet_i_want--;
    }
    if (bet_i_want > balance[player_id]) {
        bet_i_want = 0;
    }

    if (is_first == 1) {

        if (check_or_raise <= coolness_factor) {

            return bet_i_want; // first_bet
        }
        else {

            return 0; // check
        }
    }
    else {

        if (bets[player_id] == *max_element(bets.begin(), bets.end())) {
            if (bet_i_want <= bets[player_id]) {

                return 0;
            }
            else {

                return bet_i_want;
            }
        }
        if (bet_i_want * 2 < *max_element(bets.begin(), bets.end())) {

            if (bet_i_want * 5 < *max_element(bets.begin(), bets.end())) {
                return -1;
            }
            if (*max_element(bets.begin(), bets.end()) * 3 < balance[player_id]) {
                return *max_element(bets.begin(), bets.end());
            }
            else {
                return -1; // fold
            }

        }
        else if (bet_i_want > *max_element(bets.begin(), bets.end()) * 2) {

            return bet_i_want; // raise
        }
        else {

            if (balance[player_id] >= *max_element(bets.begin(), bets.end())) {
                int call_or_fold = rand() % 2;

                if (call_or_fold == 0) {

                    return -1;
                }
                else {

                    return *max_element(bets.begin(), bets.end());
                }
            }
            else {

                return -1;
            }
        }
    }

    return -1;
}

string cout_cards(vector<vector<string>> card) {
    string ret = "\n";
    if (card[0][0] == "") {
        return "";
    }

    int card_size = 0;
    for (int i = 0; i < card.size(); i++) {
        if (card[i][0] != "") {
            card_size++;
        }
        else {
            break;
        }
    }

    for (int i = 0; i < card_size; i++) {
        ret += " ___ ";
    }
    ret += "\n";
    for (int i = 0; i < card_size; i++) {
        if (card[i][0] == "10") {
            ret = ret + "|" + card[i][0] + " |";
        }
        else {
            ret = ret + "|" + card[i][0] + "  |";
        }
    }
    ret += "\n";
    for (int i = 0; i < card_size; i++) {
        ret = ret + "| " + card[i][1] + " |";
    }
    ret += "\n";
    for (int i = 0; i < card_size; i++) {
        if (card[i][0] == "10") {
            ret = ret + "|_" + card[i][0] + "|";
        }
        else {
            ret = ret + "|__" + card[i][0] + "|";
        }
    }
    ret += "\n";
    return ret;
}

string cout_closed_cards() {
    return ("\n ___  ___ \n|###||###|\n|###||###|\n|###||###|\n");
}

string cout_screen(int turn_number) {
    printf("\x1b[H\x1b[2J\x1b[3J");
    string ret = "";
    string turn_barrow = "";
    if (turn_number != -1) {
        turn_barrow = "-> ";
    }

    // Cards on the table
    ret = ret + cout_cards(cards_on_table);

    for (int i = 0; i < 25; i++) {
        ret += "*";
    }
    ret += "\n\n";

    // Bank
    ret = ret + "Bank: " + to_string(bank);
    ret += "\n\n";

    // Players
    for (int i = 0; i < 5; i++) {
        if (turn_number == i) {
            ret += turn_barrow;
        }
        string player_name = "";
        if (i == 0) {
            player_name = "You (balance: ";
        }
        else {
            player_name = "Bot_" + to_string(i) + " (balance: ";
        }
        ret = ret + player_name + to_string(balance[i]) + ") (bet: " + to_string(bets[i]) + ")";
        if (winners.empty() == 0) {

            if (count(begin(winners), end(winners), i) == 1) {
                ret += "  <======================= WINNER!!!";
            }
        }
        if (fold_players[i] == 1) {
            ret += " Fold";
        }
        if (winners.empty() == 0 and player_combinations.empty() == 0) {
            if (fold_players[i] == 0) {
                ret += "\nCombination: ";
                for (int comb = player_combinations[0].size() - 1; comb >= 0; comb--) {
                    if (player_combinations[i][comb].empty() == 0) {
                        ret += combination_names[comb];
                        break;
                    }
                }
            }
        }
        if (player_cards[0][0][0] != "") {
            if (i == 0) {
                ret += cout_cards(player_cards[i]);
            }
            else {
                if (winners.empty() == 1) {
                    ret += cout_closed_cards();
                }
                else {
                    if (fold_players[i] == 1) {
                        ret += cout_closed_cards();
                    }
                    else {
                        ret += cout_cards(player_cards[i]);
                    }
                }
            }
        }
        ret += "\n";
        //ret += "\n";

    }

    return ret;
}

void place_a_bet(int player_id, int circle_number=1) {
    if (player_id >= 5) {
        player_id %= 5;
    }

    bool are_bets_equal = 0;
    if (circle_number != 1) {
        are_bets_equal = 1;
        for (int player_id = 0; player_id < 5; player_id++) {
            if (fold_players[player_id] == 1) {
                continue;
            }
            if (bets[player_id] != *max_element(bets.begin(), bets.end())) {
                are_bets_equal = 0;
                break;
            }
        }
    }
    if (are_bets_equal == 1) {
        return;
    }
    else {
        for (int i = 0; i < 5; i++) {
            if (player_id >= 5) {
                player_id %= 5;
            }
            Sleep(sleep_time);
            cout << cout_screen(player_id);
            if (player_id == 0) {
                if (fold_players[player_id] == 1 or balance[player_id] == 0) {
                    player_id += 1;
                    continue;
                }
                int my_bet = -1;
                cout << "\nHow much do you want to bet? Your bet must be at least " << to_string(*max_element(bets.begin(), bets.end()) - bets[0]);
                cout << ". You can also type '-1' to fold\n";
                while (1 == 1) {
                    cin >> my_bet;
                    if (my_bet == -1) {
                        fold_players[0] = 1;
                        my_bet = 0;
                        break;
                    }
                    if (my_bet - balance[0] < 0) {
                        cout << "No money!";
                    }
                    if (my_bet % 5 == 0) {
                        if (my_bet + bets[0] >= *max_element(bets.begin(), bets.end())) {
                            break;
                        }
                        else {
                            cout << "You can't bet like that. Try again.\n";
                            continue;
                        }
                    }
                    else {
                        cout << "Your bet must be a multiple of 5 (0, 5, 10, 15...). Try again.\n";
                    }
                    }
                bets[0] += my_bet;
                balance[0] -= my_bet;
            }
            else {
                if (fold_players[player_id] == 1) {
                    player_id += 1;
                    continue;
                }
                bool is_first = 0;
                if (accumulate(bets.begin(), bets.end(), 0) == 0) {
                    is_first = 1;
                }
                int this_bet = calculate_bet(player_id, is_first);

                if (this_bet == -1) {
                    this_bet = 0;
                    fold_players[player_id] = 1;
                }
                else {
                    if (this_bet != 0) {
                        this_bet -= bets[player_id];
                    }
                }

                bets[player_id] += this_bet;
                balance[player_id] -= this_bet;
            }
            bool are_bets_equal = 0;
            if (circle_number != 1) {
                are_bets_equal = 1;
                for (int player_id = 0; player_id < 5; player_id++) {
                    if (fold_players[player_id] == 1) {
                        continue;
                    }
                    if (bets[player_id] != *max_element(bets.begin(), bets.end())) {
                        are_bets_equal = 0;
                        break;
                    }
                }
            }
            if (are_bets_equal == 1) {
                return;
            }
            player_id += 1;
            Sleep(sleep_time);
            cout << cout_screen(player_id);
            Sleep(sleep_time);
        }
        place_a_bet(player_id, circle_number+1);
    }
}

void determine_winner() {
    vector<int> candidates = {};
    int max_comb = 0;

    for (int player_id = 0; player_id < 5; player_id++) {
        if (fold_players[player_id] == 1) {
            continue;
        }
        for (int comb = player_combinations[0].size() - 1; comb >= 0; comb--) {
            if (player_combinations[player_id][comb].empty() == 0 and comb > max_comb) {
                max_comb = comb;
            }
        }
    }

    for (int player_id = 0; player_id < 5; player_id++) {
        if (fold_players[player_id] == 1) {
            continue;
        }
        for (int comb = player_combinations[0].size() - 1; comb >= 0; comb--) {
            if (player_combinations[player_id][comb].empty() == 0 and comb == max_comb) {
                candidates.push_back(player_id);
            }
        }
    }

    if (candidates.size() == 1) {
        winners = candidates;
    }
    else {

        if (player_combinations[candidates[0]][max_comb].size() == 1) {

            int max_max_comb = 0;
            for (int cand = 0; cand < candidates.size(); cand++) {
                if (player_combinations[candidates[cand]][max_comb][0] > max_max_comb) {
                    max_max_comb = player_combinations[candidates[cand]][max_comb][0];
                }
            }
            for (int cand = 0; cand < candidates.size(); cand++) {
                if (player_combinations[candidates[cand]][max_comb][0] == max_max_comb) {
                    winners.push_back(candidates[cand]);
                }
            }
        }

        else {

            int max_max_comb_1 = 0;
            for (int cand = 0; cand < candidates.size(); cand++) {
                if (player_combinations[candidates[cand]][max_comb][1] > max_max_comb_1) {
                    max_max_comb_1 = player_combinations[candidates[cand]][max_comb][1];
                }
            }

            for (int cand = 0; cand < candidates.size(); cand++) {
                if (player_combinations[candidates[cand]][max_comb][1] == max_max_comb_1) {
                    winners.push_back(candidates[cand]);
                }
            }

            if (winners.size() > 1) {
                int max_max_comb_0 = 0;
                for (int cand = 0; cand < candidates.size(); cand++) {
                    if (player_combinations[candidates[cand]][max_comb][0] > max_max_comb_0) {
                        max_max_comb_0 = player_combinations[candidates[cand]][max_comb][0];
                    }
                }
                for (int cand = 0; cand < candidates.size(); cand++) {
                    if (player_combinations[candidates[cand]][max_comb][0] == max_max_comb_0) {
                        winners.push_back(candidates[cand]);
                    }
                }
            }
        }
    }
}

string game(int dealer_num) {
    // Preparation
    if (dealer_num >= balance.size()) {
        dealer_num %= 5;
    }
    for (int player_id = 0; player_id < 5; player_id++) {
        fold_players[player_id] = 0;
    }

    // Start
    cout << cout_screen(-1);
    cout << "\nType 's' to start the game\n";
    string message;
    cin >> message;
    if (message == "s") {
        message = "";
    }

    // Blinds
    int small_blind_num = dealer_num + 1;
    int big_blind_num = dealer_num + 2;

    if (small_blind_num >= balance.size()) {
        small_blind_num %= 5;
    }
    if (big_blind_num >= balance.size()) {
        big_blind_num %= 5;
    }

    cout << cout_screen(small_blind_num);
    Sleep(sleep_time);
    balance[small_blind_num] -= 5;
    bets[small_blind_num] += 5;
    cout << cout_screen(small_blind_num);
    Sleep(sleep_time);

    cout << cout_screen(big_blind_num);
    Sleep(sleep_time);
    balance[big_blind_num] -= 10;
    bets[big_blind_num] += 10;
    cout << cout_screen(big_blind_num);
    Sleep(sleep_time);

    // Deal cards
    for (int weight = 0; weight < card_weights.size(); weight++) {
        for (int suit = 0; suit < card_suits.size(); suit++) {
            cards_to_use.push_back({card_weights[weight], card_suits[suit]});
        }
    }

    auto rng = default_random_engine{};
    random_device rd;
    mt19937 g(rd());
    shuffle(begin(cards_to_use), end(cards_to_use), g);

    for (int player_id = 0; player_id < 5; player_id++) {
        for (int card = 0; card < 2; card++) {
            player_cards[player_id][card] = cards_to_use[0];
            cards_to_use.erase(cards_to_use.begin());
        }
    }

    cout << cout_screen(-1);
    Sleep(sleep_time * 2);

    // Preflop
    place_a_bet(big_blind_num+1);
    bank += accumulate(bets.begin(), bets.end(), 0);
    for (int i = 0; i < 5; i++) {
        bets[i] = 0;
    }
    check_pre_winner();
    cout << cout_screen(-1);
    Sleep(sleep_time);
    if (winners.empty() == 0) {
        balance[winners[0]] += bank;
        bank = 0;
        cout << cout_screen(-1);
        cout << "Type 'c' to continue\n";
        cin >> message;
        message = "";
        return "";
    }

    // Flop
    for (int i = 0; i < 3; i++) {
        cards_on_table[i] = cards_to_use[0];
        cards_to_use.erase(cards_to_use.begin());
    }
    cout << cout_screen(-1);
    Sleep(sleep_time);

    // Flop bets
    place_a_bet(big_blind_num + 1);
    bank += accumulate(bets.begin(), bets.end(), 0);
    for (int i = 0; i < 5; i++) {
        bets[i] = 0;
    }

    check_pre_winner();

    cout << cout_screen(-1);

    Sleep(sleep_time);
    if (winners.empty() == 0) {

        balance[winners[0]] += bank;

        bank = 0;
        cout << cout_screen(-1);
        cout << "Type 'c' to continue\n";
        cin >> message;
        message = "";
        return "";
    }

    // Turn
    cards_on_table[3] = cards_to_use[0];
    cards_to_use.erase(cards_to_use.begin());
    cout << cout_screen(-1);
    Sleep(sleep_time);

    // Turn bets
    place_a_bet(big_blind_num + 1);
    bank += accumulate(bets.begin(), bets.end(), 0);
    for (int i = 0; i < 5; i++) {
        bets[i] = 0;
    }
    check_pre_winner();
    cout << cout_screen(-1);
    Sleep(sleep_time);
    if (winners.empty() == 0) {
        balance[winners[0]] += bank;
        bank = 0;
        cout << cout_screen(-1);
        cout << "Type 'c' to continue\n";
        cin >> message;
        message = "";
        return "";
    }

    // River
    cards_on_table[4] = cards_to_use[0];
    cards_to_use.erase(cards_to_use.begin());
    cout << cout_screen(-1);
    Sleep(sleep_time);

    // River bets
    place_a_bet(big_blind_num + 1);
    bank += accumulate(bets.begin(), bets.end(), 0);
    for (int i = 0; i < 5; i++) {
        bets[i] = 0;
    }
    check_pre_winner();
    cout << cout_screen(-1);
    Sleep(sleep_time);
    if (winners.empty() == 0) {
        balance[winners[0]] += bank;
        bank = 0;
        cout << cout_screen(-1);
        cout << "Type 'c' to continue\n";
        cin >> message;
        message = "";
        return "";
    }

    // Check combinations
    for (int player_id = 0; player_id < 5; player_id++) {
        player_combinations.push_back({});
        check_combinations(player_id);
    }
    cout << cout_screen(-1);
    Sleep(sleep_time);
    
    // Determine the winner
    determine_winner();

    cout << cout_screen(-1);
    cout << "Type 'c' to continue\n";
    cin >> message;
    message = "";
    if (winners.size() > 1) {
        int bank_1 = bank / 5;
        int each_pay = (bank_1 / winners.size()) * 5;
        for (int winner_number = 0; winner_number < winners.size(); winner_number++) {
            balance[winners[winner_number]] += each_pay;
            if (winner_number == 0) {
                balance[winners[winner_number]] += (bank - (each_pay * winners.size()));
            }
        }
    }
    balance[winners[0]] += bank;
    bank = 0;

    return "";
}

int main()
{
    int dealer_id = 0;
    while (1 == 1) {
        game(dealer_id);
        bool end_game = 0;
        for (int player_id = 0; player_id < 5; player_id++) {
            if (balance[player_id] == 0) {
                cout << "Player " + to_string(player_id) + " is looser!\n\nGAME OVER";
                end_game = 1;
                break;
            }
        }
        if (end_game == 1) {
            return 0;
        }
        dealer_id += 1;
        winners = {};
        player_combinations = {};
        player_cards = { {{"", ""}, {"", ""}}, {{"", ""}, {"", ""}}, {{"", ""}, {"", ""}}, {{"", ""}, {"", ""}}, {{"", ""}, {"", ""}} };
        cards_to_use = {};
        cards_on_table = { {"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""} };
        fold_players = { 0, 0, 0, 0, 0 };
    }
    return 0;
}
