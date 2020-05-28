import { TestBed } from '@angular/core/testing';

import { SetupWifiNotCompletedGuard } from './setup-wifi-not-completed.guard';

describe('SetupWifiNotCompletedGuard', () => {
  let guard: SetupWifiNotCompletedGuard;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    guard = TestBed.inject(SetupWifiNotCompletedGuard);
  });

  it('should be created', () => {
    expect(guard).toBeTruthy();
  });
});
