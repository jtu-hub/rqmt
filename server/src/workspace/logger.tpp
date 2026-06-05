template<typename T>
void Logger::log(const std::string& log_type, const T& message) {

    std::ofstream& base = *this;
    std::string log_header = "[" + log_type + "]";

    base << std::left << std::setw(padded_log_type_width) << log_header;
    base << message << std::endl;
}
