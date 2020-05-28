import { TestBed } from '@angular/core/testing';

import { SetupWifiCompletedGuard } from './setup-wifi-completed.guard';

describe('SetupWifiCompletedGuard', () => {
  let guard: SetupWifiCompletedGuard;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    guard = TestBed.inject(SetupWifiCompletedGuard);
  });

  it('should be created', () => {
    expect(guard).toBeTruthy();
  });
});
