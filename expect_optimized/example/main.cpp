//
// Copyright (c) 2021 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
struct Manager {            // Indescriptive Naming
  static auto& attendees(); // Singleton
  static auto& speakers();  // Singleton
  ...
};

struct Session {
  [[gnu::always_inline]] // Premature Optimization
  auto run();
  ...
};

class cppcon_talk {
 public:
  auto run(const auto& speaker, const auto&... attendees) {

   // Untestability
   Manager::speakers().get_speakers().get(speaker).join(session);

   // Duplication
   (Manager::attendees().get_attendees().get(attendees).join(session), ...); 
   return session.run();
  }

 private:
  std::shared_ptr<Session> session // Tight Coupling
    = std::make_shared<Session>();
};
