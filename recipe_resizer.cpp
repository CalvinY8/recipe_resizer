/*problem:
When cooking, the recipe is usually the wrong size, and in imperial.
It is tiresome to enter data into an online recipe resizer.
This tool will take all lines, convert to metric and resize.

USAGE:
copy the recipe list from Allrecipes webpage into a txt file.
specify parameters in main()
press run.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <set>
#include <map>
#include <tuple>
#include <math.h>

/*recipe resizer
input is lines of a txt file

0) read the lines, ignore if line is blank [x]
1) convert to metric [x]
2) resize by certain factor, round to 2 decimal places [x]
3) test this compared to entering data manually [x]
*/

std::string convert_vulgar_fraction_to_decimal(std::string test_str){
    
    //[⅒ ½ ⅓ ¼ ⅕ ⅙ ⅐ ⅛ ⅑ ⅔ ⅖ ¾ ⅗ ⅜ ⅘ ⅚ ⅝ ⅞]

    std::map<std::string, std::string> convert_vulgar_fractions{
        {"⅒", ".1"},
        {"½", ".5"},
        {"⅓", ".333"},
        {"¼", ".25"},
        {"⅕", ".2"},
        {"⅙", ".167"},
        {"⅐", ".143"},
        {"⅛", ".125"},
        {"⅑", ".111"},
        {"⅔", ".667"},
        {"⅖", ".4"},
        {"¾", ".75"},
        {"⅗", ".6"},
        {"⅜", ".375"},
        {"⅘", ".8"},
        {"⅚", ".833"},
        {"⅝", ".625"},
        {"⅞", ".875"}
        };

    /*
    regex match any vulgar fraction

    use the vulgar fraction as key to access the map, getting the decimal equivalent
    */

    std::regex vulgar_fraction_reg ("[^A-Za-z0-9, \\-\\(\\)]");

    std::string vulgar_fraction;

    int vulgar_fraction_index;

    std::smatch match;

    if (std::regex_search(test_str, match, vulgar_fraction_reg)){
        //std::cout << "match found: " << match.str(0) << std::endl;
        //if inside this if statement, match found

        //extracting the matched data was troublesome
        //don't do either of these, I've tried them and they don't work:
        //getting match.str(0), which gives the whole match
        //using match.position(), which gives the index of the matched character, then use test_str[index]

        //---how to get vulgar_fraction from test_str
        //find substring inside a string
        //but get 3 characters, not just 1

        vulgar_fraction_index = match.position();

        //std::cout << vulgar_fraction_index << std::endl;

        //std::cout << test_str.substr(vulgar_fraction_index, 3) << std::endl;

        vulgar_fraction = test_str.substr(vulgar_fraction_index, 3); //slice the initial string. the vulgar fraction character may be 1, 2, or 3 characters long
        //taking the worst case, which is length 3, that means there may be trailing whitespace in the substring

        //trim trailing whitespace
        //https://stackoverflow.com/a/65404298
        vulgar_fraction.erase(vulgar_fraction.find_last_not_of(" ") + 1);
        //^find the index of the last element that is not a space, then erase starting from one index past that

        //std::cout << vulgar_fraction[vulgar_fraction.length() - 1] << std::endl;

        /*
        myString.erase(first_index, 2nd_index)  erases the character at the first_index, and stop before the 2nd index.
        So, erase(0,1) -> erases first character only.
        */

        std::string replace_with_this = convert_vulgar_fractions[vulgar_fraction];

        //std::cout << replace_with_this << std::endl;

        test_str = std::regex_replace(test_str, std::regex(vulgar_fraction), replace_with_this);
        // replace ' ⅒' -> '.1'
        // "1 ½ cup peanuts" -> "1.5 cup peanuts"

        //previous step turns "1 ½ cup" into "1 .5 cup"
        //this next step further turns it into "1.5 cup"
        test_str = std::regex_replace(test_str, std::regex(" \\."), ".");

    }

    //std::cout << test_str << std::endl;
    return test_str;
}

//return quantity in ml
float convert_imperial_to_metric_for_liquids(float quantity, std::string imperial_unit){
    //(gallon|quart|pint|cup|fluid ounce|tablespoon|teaspoon) x (conversion_factor) -> ml
    //imperial unit determines the conversion factor to use on quantity
    //https://stackoverflow.com/questions/15151480/simple-dictionary-in-c
    std::map<std::string, float> conversion_ratios_imperial_to_metric{
        {"gallon", 3785.41},
        {"quart", 946.353},
        {"pint", 568.261},
        {"cup", 236.588},
        {"fluid ounce", 29.5735},
        {"tablespoon", 14.7868},
        {"teaspoon", 4.92892}
        };
        
        //use conversion unit as key to access dict and get the conversion_factor
        float conversion_factor = conversion_ratios_imperial_to_metric[imperial_unit];
        quantity *= conversion_factor;
        return quantity;
}

//return quantity in g
float convert_imperial_to_metric_for_solids(float quantity, std::string imperial_unit){
    //(pound|ounce|"") x (conversion_factor) -> g
    //imperial unit determines the conversion factor to use on quantity
    //https://stackoverflow.com/questions/15151480/simple-dictionary-in-c
    std::map<std::string, float> conversion_ratios_imperial_to_metric{
        {"pound", 453.592},
        {"ounce", 28.3495}
        };

        //use conversion unit as key to access dict and get the conversion_factor
        float conversion_factor = conversion_ratios_imperial_to_metric[imperial_unit];
        quantity *= conversion_factor;
        return quantity;
}

//see if is solid or liquid, call either "convert_imperial_to_metric_for_solids" or "convert_imperial_to_metric_for_liquids"
std::tuple <float, std::string> convert_imperial_to_metric(float quantity, std::string imperial_unit){

    float quantity_in_metric;

    std::tuple <float, std::string> result; //tuple containing <quantity, metric unit("g" or "ml")>


    //https://stackoverflow.com/questions/14515274/how-to-check-if-a-string-is-in-a-list-of-strings
    std::set<std::string> imperial_units_liquids = {"gallon","quart","pint","cup","fluid ounce","tablespoon","teaspoon"};

    if (imperial_units_liquids.find(imperial_unit) != imperial_units_liquids.end()){

        quantity_in_metric = convert_imperial_to_metric_for_liquids(quantity, imperial_unit);

        result = std::make_tuple(quantity_in_metric, "ml");

        return result;
    }
    //^the regex in read_ingredients_from_txt guarantees any quantity will only ever be an imperial unit, if its not liquid, must be solid

    quantity_in_metric = convert_imperial_to_metric_for_solids(quantity, imperial_unit);

    result = std::make_tuple(quantity_in_metric, "g");

    return result;
}

std::tuple<float, std::string> get_grams_given_food_in_container(float number_of_containers, std::string string_food_in_container){
    //given string, use regex on string_food_in_container to identify the following quantities
    
    std::string imperial_unit;
    float weight_per_container_imperial;

    float weight_per_container_metric;

    /*
    examples of input
    1 (28 ounce) can crushed tomatoes
    0.5 (14 ounce) can sweetened condensed milk
    10 (12-ounce) package bacon
    3.25 (1.4 ounce) bars chocolate
    */

    /*pattern is:
    number_of_containers (weight_per_container_metric imperial_unit) string
    get number_of_containers by regex_search
    get weight_per_container_metric by delimiter spaces, then get
    get imperial_unit by regex_search
    */

    std::regex weight_per_container_imperial_reg ("\\(\\d*\\.*\\d*");

    std::regex imperial_unit_reg ("(pound|ounce|gallon|quart|pint|cup|fluid ounce|tablespoon|teaspoon)"); //reused line, consider making to global var

    std::smatch match;

    if (std::regex_search(string_food_in_container, match, weight_per_container_imperial_reg)){
            std::string str = match.str(0); // "(28" is an example of what the regex extracts
            str.erase(0, 1); //remove parenthesis, the above example would become "28"
            weight_per_container_imperial = std::stof(str); //convert string to float
            //std::cout << match.str(0) << std::endl; //this is the weight per container
    }
    
    if (std::regex_search(string_food_in_container, match, imperial_unit_reg)){
            //std::cout << match.str(0);
            imperial_unit = match.str(0);
    }
    
    std::tuple <float, std::string> result; //store result from the following function call:
    result = convert_imperial_to_metric(weight_per_container_imperial, imperial_unit); //returns tuple containing <quantity, metric unit("g" or "ml")>

    //std::cout << "weight of container imperial" << weight_per_container_imperial << std::endl;

    //std::cout << "weight of each container" << std::get<0>(result) << ", units in metric" << std::get<1>(result) << std::endl;

    weight_per_container_metric = std::get<0>(result);

    float food_weight_metric = number_of_containers * weight_per_container_metric;
    //for example, 0.5 (1 oz) can of tomatoes = 0.5 * 1 oz of tomatoes
    
    return std::make_tuple(food_weight_metric, std::get<1>(result));
}

void read_ingredients_from_txt(std::string input_filename_no_extension, std::string output_filename_no_extension, float input_servings, float output_servings){
    std::string quantity_string;
    float quantity;

    std::string metric_unit = "";

    std::string quantity_and_imperial_unit;

    std::string ingredient_name = "";

    std::tuple <float, std::string> result; //tuple containing <quantity, metric unit("g" or "ml")>

    //https://stackoverflow.com/questions/13035674/how-to-read-a-file-line-by-line-or-a-whole-text-file-at-once
    std::ifstream input_file(input_filename_no_extension + ".txt");
    std::ofstream output_file(output_filename_no_extension + ".txt");

    std::string str;

    std::smatch match_quantity; //https://www.geeksforgeeks.org/smatch-regex-regular-expressions-in-c/
    std::smatch match_imperial_unit;

    while (std::getline(input_file, str))
    {
        if (str.empty()){
            continue; //skip empty lines
        }

        //std::cout << str << std::endl; //print to see unprocessed str read correctly
        
        //replace the vulgar fraction with decimal
        std::string processed_str = convert_vulgar_fraction_to_decimal(str);

        //std::cout << processed_str << std::endl; //print to see str processed correctly

        //A) get the quantity
        std::regex quantity_reg ("^\\d*\\.*\\d*");
        if(std::regex_search(processed_str, match_quantity, quantity_reg)){
            //std::cout << match.str(0) << std::endl; //this is the quantity
            quantity_string = match_quantity.str(0);
            //std::cout << quantity_string << std::endl;
            quantity = std::stof(quantity_string);
        }

        //B) get the metric_units
        std::regex in_container_reg ("^\\d*\\.*\\d* \\(\\d+.(pound|ounce|gallon|quart|pint|cup|fluid ounce|tablespoon|teaspoon)\\) [A-Za-z]");
        std::regex imperial_unit_reg ("(pound|ounce|gallon|quart|pint|cup|fluid ounce|tablespoon|teaspoon)s*");

        if(std::regex_search(processed_str, in_container_reg)){
            /*
            1 (28 ounce) can crushed tomatoes
            ½ (14 ounce) can sweetened condensed milk
            10 (12-ounce) package bacon
            3 ¼ (1.4 ounce) bars chocolate
            */

            //std::cout << "ingredient is in container" << std::endl;
        
            //get ingredient name
            //index of first character that is a bracket
            int index_of_first_bracket = processed_str.find_first_of(")");
            
            //index of last character in string
            int index_of_last_char = processed_str.length() - 1;
            
            ingredient_name = processed_str.substr(index_of_first_bracket + 2, index_of_last_char);

            //std::cout << ingredient_name << std::endl;


            result = get_grams_given_food_in_container(quantity, processed_str);
            //this function returns total food in container, in grams
            //ex: 170.097 g given 1/2 (12 ounce can) tomatoes
            //std::cout << std::get<0>(result) << " " << std::get<1>(result) << " " << ingredient_name << std::endl; //-------------A
            quantity = std::get<0>(result);
            metric_unit = std::get<1>(result);

        } else if (std::regex_search(processed_str, match_imperial_unit, imperial_unit_reg)){
            //std::cout << processed_str << std::endl;
            /*
            examples:
            1 ½ cups all-purpose flour
            1 teaspoon baking soda
            ⅔ cup butter
            */

            std::string imperial_unit;

            //std::cout << "ingredient is direct imperial unit" << std::endl;

            //std::cout << match.str(0); //this is the imperial units
            imperial_unit = match_imperial_unit.str(0);


            //***get ingredient_name

            //since we regex matched the imperial unit in the string,

            int imperial_unit_position = match_imperial_unit.position(); //get position

            int imperial_unit_length = imperial_unit.length(); //get length

            //slice the sentence from length + position onward

            //ingredient_name = processed_str.substr(imperial_unit_position + imperial_unit_length);
            ingredient_name = processed_str.substr(imperial_unit_position + imperial_unit_length + 1);
            //add one more to strip whitespace following the imperial unit

            //***get ingredient_name end


            //if imperial_unit ends in "s", remove the "s"

            char imperial_unit_last_char_index = imperial_unit.length() - 1;

            if (imperial_unit[imperial_unit_last_char_index] == 's'){
            
                imperial_unit.erase(imperial_unit_last_char_index, imperial_unit_last_char_index + 1);
            
            }

            result = convert_imperial_to_metric(quantity, imperial_unit); //result is tuple

            quantity = std::get<0>(result);
            metric_unit = std::get<1>(result);

            //std::cout << quantity << "|" << metric_unit << "|" << ingredient_name << std::endl; //-------------B

        }else{
            //std::cout << "ingredient is each" << std::endl;
            /*examples:
                2 eggs
                1 clove garlic
            */

            //get ingredient_name, which is everything past the quantity and one additional space
            ingredient_name = processed_str.substr(match_quantity.position() + quantity_string.length() + 1);

            //std::cout << match_quantity.position() << "|" << quantity_string.length() << std::endl;

            //std::cout << quantity << "|" << ingredient_name << std::endl; //-------------C
            metric_unit = "each";
        }

        //std::cout << quantity << "|" << metric_unit << "|" << ingredient_name << std::endl; //-------------print to see 3 data fields before resize

        float resized_quantity = quantity * (output_servings/input_servings);

        //https://stackoverflow.com/questions/1343890/how-do-i-restrict-a-float-value-to-only-two-places-after-the-decimal-point-in-c
        float rounded_resized_quantity = roundf(resized_quantity * 100)/100;

        //std::cout << rounded_resized_quantity << std::endl;

        std::cout << rounded_resized_quantity << "|" << metric_unit << "|" << ingredient_name << std::endl; //-------------print to see 3 data fields after resize

        //write each line to output file
        output_file << rounded_resized_quantity << "|" << metric_unit << "|" << ingredient_name << std::endl;
    }

}



int main()
{
    std::string input_filename_no_extension = "recipe"; //must have a txt file of this name in same directory
    std::string output_filename_no_extension = "resized_recipe"; //txt will be created if not exist

    float input_servings = 8; //how many servings did the recipe originally serve?
    float output_servings = 1; //how many servings do you want?
    
    read_ingredients_from_txt(input_filename_no_extension, output_filename_no_extension, input_servings, output_servings);

    // std::string test = "1 ⅒ cups peanuts";
    // std::string processed_str = convert_vulgar_fraction_to_decimal(test);
    // std::cout << processed_str << std::endl;

    // float quantity = 1.1;
    // float quantity_metric = convert_imperial_to_metric_for_liquids(quantity, "cup");
    // std::cout << quantity_metric << " ml" << std::endl;


    //***start
    // check your whitespace trimming method works to correctly identify and replace all 18 possible vulgar fractions.
    // (a loop might be handy for this)
    // std::vector<std::string> vulgar_fractions = {"⅒","½","⅓","¼","⅕","⅙","⅐","⅛","⅑","⅔","⅖","¾","⅗","⅜","⅘","⅚","⅝","⅞"};
    
    // for (int i = 0; i < 18; i++){
    //     std::string vulgar_fraction = vulgar_fractions[i];
    //     std::string test_str = convert_vulgar_fraction_to_decimal("12 " + vulgar_fraction + " (28 ounce) can crushed tomatoes");
    //     std::cout << test_str << std::endl;
    // }

    //try it this way "1/2 can tomatoes"
    //and also this way "1 1/2 can tomatoes"
    //***end

    return 0;
}
