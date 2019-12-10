import {app, Button, Composite, contentView, NavigationView, Page, Tab, TabFolder, TextView} from 'tabris';

// demonstrates NavigationViews as children of a TabFolder

const tabFolder = new TabFolder({
  left: 0, top: 0, right: 0, bottom: 0,
  tabBarLocation: 'bottom',
  background: 'white'
}).appendTo(contentView);

function createTab(title, image) {
  const tab = new Tab({title, image})
    .appendTo(tabFolder);
  const navigationView = new NavigationView({
    left: 0, top: 0, right: 0, bottom: 0
  }).appendTo(tab);
  createPage(navigationView, title);
}

function createPage(navigationView, title) {
  const text = title || 'Page ' + (navigationView.pages().length + 1);
  const page = new Page({
    title: text,
    background: '#02eeb1'
  }).appendTo(navigationView);
  const controls = new Composite({
    centerX: 0, centerY: 0
  }).appendTo(page);
  new TextView({
    centerX: 0,
    text
  }).appendTo(controls);
  new Button({
    top: 'prev() 16', centerX: 0,
    text: 'Add Page'
  }).onSelect(() => createPage(navigationView))
    .appendTo(controls);
  new Button({
    top: 'prev() 16', centerX: 0,
    text: 'Remove Page'
  }).onSelect(() => page.dispose())
    .appendTo(controls);
}

createTab('Cart', 'icons/map@2x.svg');
createTab('Pay', 'resources/card@2x.png');
createTab('Statistic', 'resources/chart@2x.png');

app.onBackNavigation((event) => {
  // handle the "physical" back button on Android
  const navigationView = tabFolder.selection.find(NavigationView).only();
  const page = navigationView.pages().last();
  if (page !== undefined) {
    page.dispose();
    event.preventDefault();
  }
});
