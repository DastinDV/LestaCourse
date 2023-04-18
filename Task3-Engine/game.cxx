#include "game.hxx"
#include <filesystem>
#include <iostream>

namespace core {

Game *ReloadGame(Game *old, const char *library_name,
                 const char *tmp_library_name, Engine &engine,
                 void *&old_handle) {

  using namespace std::filesystem;

  if (old) {
    SDL_UnloadObject(old_handle);
    delete old;
    old = NULL;
  }

  if (std::filesystem::exists(tmp_library_name)) {
    if (0 != remove(tmp_library_name)) {
      std::cerr << "error: can't remove: " << tmp_library_name << std::endl;
      return nullptr;
    }
  }

  try {
    copy(library_name, tmp_library_name); // throw on error
  } catch (const std::exception &ex) {
    std::cerr << "error: can't copy [" << library_name << "] to ["
              << tmp_library_name << "]" << std::endl;
    return nullptr;
  }

  void *game_handle = SDL_LoadObject(tmp_library_name);

  if (game_handle == nullptr) {
    std::cerr << "error: failed to load: [" << tmp_library_name << "] "
              << SDL_GetError() << std::endl;
    return nullptr;
  }

  old_handle = game_handle;

  SDL_FunctionPointer create_game_func_ptr =
      SDL_LoadFunction(game_handle, "CreateGame");

  if (create_game_func_ptr == nullptr) {
    std::cerr << "error: no function [CreateGame] in " << tmp_library_name
              << " " << SDL_GetError() << std::endl;
    return nullptr;
  }
  // void* destroy_game_func_ptr = SDL_LoadFunction(game_handle,
  // "destroy_game");

  using create_game_ptr = decltype(&CreateGame);

  auto create_game_func =
      reinterpret_cast<create_game_ptr>(create_game_func_ptr);

  Game *game = create_game_func();

  if (game == nullptr) {
    std::cerr << "error: func [CreateGame] returned: nullptr" << std::endl;
    return nullptr;
  }
  return game;
}

} // namespace core