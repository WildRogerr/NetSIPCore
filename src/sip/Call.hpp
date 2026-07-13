#pragma once
#include <pjsua2.hpp>
#include <functional>
#include <memory>



class SIPCall : public pj::Call
{
    public:

        SIPCall(pj::Account& acc, int call_id = PJSUA_INVALID_ID);
        ~SIPCall();
        virtual void onCallState(pj::OnCallStateParam &prm) override;
        void onCallMediaState(pj::OnCallMediaStateParam &prm) override;
        std::function < void(std::shared_ptr <pj::Call>) > incomingCallCallback;
        std::function < void(const std::string&, const std::string&, const std::string&) > stateCallback;
        std::function < void() > audioFinishedCallback;
        int getCallId();
        void setSpeakerState(bool state);
        void setMicrophoneState(bool state);
        void playAudio(const std::string& path);
        void setAudioFinishedCallback(std::function<void() > cb);
        std::string getRemoteUri();

    private:

        pj::AudioMedia* currentAudioMedia = nullptr;
        bool mediaConnected = false;
        bool speakerEnabled = true;
        bool microphoneEnabled = true;

};