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

template<SessionLike TSession>
class cppcon_talk {
 public:
   explicit(true) cppcon_talk(TSession& session)
     : session_{session} {
       speaker.join(session);
       (attendees.join(session), ...);
     }

   auto run() {
     session_.run();
   }

 private:
  TSession& session_;
};

int main() {
  SpeakerLike speaker     = {"Kris"};
  AttendeesLike attendees = {{"John"}, {"Mike"}};
  SessionLike session     = {speaker, attendees};
  cppcon_talk talk        = {session};
  talk.run();
}
