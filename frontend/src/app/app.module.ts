import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { ReactiveFormsModule } from '@angular/forms';
import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { NotificationsComponent } from './notifications/notifications.component';
import { SettingsComponent } from './settings/settings.component';
import { LoadingComponent } from './loading/loading.component';
import { SetupWifiComponent } from './setup-wifi/setup-wifi.component';
import { SetupMqttComponent } from './setup-mqtt/setup-mqtt.component';
import { DashboardComponent } from './dashboard/dashboard.component';
import { SignalStatusComponent } from './signal-status/signal-status.component';
// import { fakeBackendProvider } from './fakeBackendProvider';
import { HttpClientModule } from '@angular/common/http';
import { InputWrapperComponent } from './input-wrapper/input-wrapper.component';
import { InputStylerDirective } from './input-styler.directive';
import { ButtonStylerDirective } from './button-styler.directive';
import { SetupGsmComponent } from './setup-gsm/setup-gsm.component';
import { SetupFinishedComponent } from './setup-finished/setup-finished.component';

@NgModule({
  declarations: [
    AppComponent,
    NotificationsComponent,
    SettingsComponent,
    LoadingComponent,
    SetupWifiComponent,
    SetupMqttComponent,
    DashboardComponent,
    SignalStatusComponent,
    InputWrapperComponent,
    InputStylerDirective,
    ButtonStylerDirective,
    SetupGsmComponent,
    SetupFinishedComponent
  ],
  imports: [
    ReactiveFormsModule,
    BrowserModule,
    AppRoutingModule,
    HttpClientModule
  ],
  providers: [
    
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
