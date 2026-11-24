import wectl.web.config;
import wectl.web.assembler;
import wectl.web.writer;

int main() {
    auto html = wectl::web::build_full_html();
    return wectl::web::write_output(wectl::web::cfg::OUTPUT_FILE, html) ? 0 : 1;
}
