#pragma once
#include "controllers/ReleaseController.h"

class ReleaseView {
public:
    explicit ReleaseView(ReleaseController* ctrl);
    void run();
private:
    void showConfirmed() const;
    void showRelease();
    ReleaseController* m_ctrl;
};
