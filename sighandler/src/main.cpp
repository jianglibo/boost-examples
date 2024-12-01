#include <csignal>
#include <iostream>

void signalHandler(int signal) {
  if (signal == SIGSEGV) {
    std::cerr << "Caught segmentation fault (SIGSEGV)" << std::endl;
    // Clean up resources if needed
    std::exit(EXIT_FAILURE);  // Exit the program gracefully
  }
}

int main(int argc, char* argv[]) {
  std::signal(SIGSEGV, signalHandler);

  // Deliberate segmentation fault
  try {
    int* ptr = nullptr;
    *ptr = 42;  // This will trigger SIGSEGV
  } catch (...) {
    std::cerr << "catch(...)\n";
  }
  std::cout << "Program continues after the segmentation fault" << std::endl;

  return 0;
}