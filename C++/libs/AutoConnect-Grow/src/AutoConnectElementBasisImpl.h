/**
 * Implementation of AutoConnectElementBasis classes.
 * @file AutoConnectElementImpl.h
 * @author hieromon@gmail.com
 * @version  0.9.7
 * @date 2018-12-29
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTELEMENTBASISIMPL_H_
#define _AUTOCONNECTELEMENTBASISIMPL_H_

#include "AutoConnectElementBasis.h"
#if defined(ARDUINO_ARCH_ESP8266)
#include <regex.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <regex>
#endif

/**
 * Generate an HTML <button> element. The onclick behavior depends on
 * the code held in factionf member.
 * @return  An HTML string.
 */
const String AutoConnectButtonBasis::toHTML(void) const {
  return String(F("<button type=\"button\" name=\"")) + name + String(F("\" value=\"")) + value + String(F("\" onclick=\"")) + action + String("\">") + value + String(F("</button>"));
}

/**
 * Generate an HTML <input type=checkbox> element.
 * A "value" is associated with the input tag and sent by the form
 * action as the value of "name". If the label member is contained, it
 * is placed to the right side of the checkbox to be labeled.
 * f the label member is empty, only the checkbox is placed.
 * @return  An HTML string.
 */
const String AutoConnectCheckboxBasis::toHTML(void) const {
  String  html;

  html = String(F("<input type=\"checkbox\" name=\"")) + name + String(F("\" value=\"")) + value + String("\"");
  if (checked)
    html += String(F(" checked"));
  if (label.length())
    html += String(F(" id=\"")) + name + String(F("\"><label for=\"")) + name + String("\">") + label + String(F("</label"));
  html += String(F("><br>"));
  return html;
}

/**
 * Generate an HTML <input type=text> element.
 * If the value member is contained, it is reflected in the placeholder
 * attribute. The entered value can be obtained using the user callback
 * function registered by AutoConnectAux::on after the form is sent in
 * combination with AutoConnectSubmit.
 * @return String  an HTML string.
 */
const String AutoConnectInputBasis::toHTML(void) const {
  String  html = String("");

  if (label.length())
    html = String(F("<label for=\"")) + name + String("\">") + label + String(F("</label>"));
  html += String(F("<input type=\"text\" id=\"")) + name + String(F("\" name=\"")) + name + String("\"");
  if (pattern.length())
    html += String(F(" pattern=\"")) + pattern + String("\"");
  if (placeholder.length())
    html += String(F(" placeholder=\"")) + placeholder + String("\"");
  if (value.length())
    html += String(F(" value=\"")) + value + String("\"");
  html += String(F("><br>"));

  return html;
}

/**
 * Evaluate the pattern as a regexp and return whether value matches.
 * Always return true if the pattern is undefined.
 * @return true  The value matches a pattern.
 * @return false The value does not match a pattern.
 */
bool AutoConnectInputBasis::isValid(void) const {
  bool  rc = true;
  if (pattern.length()) {
#if defined(ARDUINO_ARCH_ESP8266)
    regex_t preg;
    if (regcomp(&preg, pattern.c_str(), REG_EXTENDED) != 0) {
      AC_DEBUG("%s regex compile failed\n", pattern.c_str());
      rc = false;
    }
    else {
      regmatch_t  p_match[1];
      rc = regexec(&preg, value.c_str(), 1, p_match, 0) == 0 ? true : false;
      regfree(&preg);
    }
#elif defined(ARDUINO_ARCH_ESP32)
    const std::regex  re(pattern.c_str());
    rc = std::regex_match(value.c_str(), re);
#endif
  }
  return rc;
}

/**
* Indicate an entry with the specified value in the value's collection.
* @param value     The value to indicates in the collection.
*/
void AutoConnectRadioBasis::check(const String& value) {
  for (std::size_t n = 0; n < _values.size(); n++) {
    if (at(n).equalsIgnoreCase(value)) {
      checked = n + 1;
      break;
    }
  }
}

/**
 * Clear value items of AutoConnectRadio and reallocate new storage.
 * All hold items are released.
 * @param reserve  If 'reserve' is greater than 0, this function
 * allocates new holding storage with the value.
 */
void AutoConnectRadioBasis::empty(const size_t reserve) {
  _values.clear();
  std::vector<String>().swap(_values);
  if (reserve)
    _values.reserve(reserve);
}

/**
 * Generate an HTML <input type=radio> element with an <option> element.
 * @return String  an HTML string.
 */
const String AutoConnectRadioBasis::toHTML(void) const {
  String  html = String("");

  if (label.length()) {
    html = label;
    if (order == AC_Vertical)
      html += String(F("<br>"));
  }
  uint8_t n = 0;
  for (const String value : _values) {
    n++;
    String  id = name + "_" + String(n);
    html += String(F("<input type=\"radio\" name=\"")) + name + String(F("\" id=\"")) + id + String(F("\" value=\"")) + value + String("\"");
    if (n == checked - 1)
      html += String(F(" checked"));
    html += String(F("><label for=\"")) + id + String("\">") + value + String(F("</label>"));
    if (order == AC_Vertical)
      html += String(F("<br>"));
  }
  return html;
}

/**
 * Returns current selected value in the radio same group
 */
const String& AutoConnectRadioBasis::value() const {
  static const String _nullString = String();
  return checked ? _values.at(checked - 1) : _nullString;
}

/**
 * Clear option items of AutoConnectSelect and reallocate new storage.
 * All hold items are released.
 * @param reserve  If 'reserve' is greater than 0, this function
 * allocates new holding storage with the value.
 */
void AutoConnectSelectBasis::empty(const size_t reserve) {
  _options.clear();
  std::vector<String>().swap(_options);
  if (reserve)
    _options.reserve(reserve);
}

/**
 * Generate an HTML <select> element with an <option> element.
 * The attribute value of the <option> element is given to the
 * AutoConnectSelect class as a string array, which would be stored
 * in the 'options' member. If a label member is contained, the <label>
 * element would be generated the preface of <select>.
 * @return String  an HTML string.
 */
const String AutoConnectSelectBasis::toHTML(void) const {
  String  html = String("");

  if (label.length())
    html = String(F("<label for=\"")) + name + String("\">") + label + String(F("</label>"));
  html += String(F("<select name=\"")) + name + String(F("\" id=\"")) + name + String("\">");
  for (const String option : _options)
    html += String(F("<option value=\"")) + option + "\">" + option + String(F("</option>"));
  html += String(F("</select>"));
  return html;
}

/**
 * Generate an HTML <input type=button> element. This element is used
 * for form submission. An 'onclick' attribute calls fixed JavaScript
 * code as 'sa' named and it's included in the template.
 * @return String  an HTML string.
 */
const String AutoConnectSubmitBasis::toHTML(void) const {
  return String(F("<input type=\"button\" name=\"")) + name + String(F("\" value=\"")) + value + String(F("\" onclick=\"_sa('")) + uri + String("')\">");
}

/**
 * Generate an HTML text element from a string of the value member. If a style
 * exists, it gives a style attribute.
 * @return String  an HTML string.
 */
const String AutoConnectTextBasis::toHTML(void) const {
  return String(F("<div style=\"")) + style + String("\">") + value + String(F("</div>"));
}

#endif // _AUTOCONNECTELEMENTBASISIMPL_H_
