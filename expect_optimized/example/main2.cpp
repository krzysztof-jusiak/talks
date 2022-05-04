//
// Copyright (c) 2021 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
struct Session {
  auto run();
};

template<Speaker TSpeaker, Attendees TAttendees>
class cppcon_talk {
 public:
   cppcon_talk(TSpeaker& speaker, TAttendees& attendees)
     : speaker_{speaker}, attendees{attendees}
   { 
     speaker.join(
   }

   auto run() {
     speaker.jon

   Manager::speakers().get_speakers().get(speaker).join(session);

   // Duplication
   (Manager::attendees().get_attendees().get(attendees).join(session), ...); 
   return session.run();
  }

 private:
  std::shared_ptr<Session> session // Tight Coupling
    = std::make_shared<Session>();
};
