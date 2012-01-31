<?php

/**
 * This class is responsible for communicating with the Monotype service via
 * the Monotype API.
 */
class FontManagementMonotype {
  private static $API_URI = 'http://api.fonts.com';
  protected static $instance = NULL;
  protected static $accounts = array();
  protected static $projects = array();
  protected static $fonts = array();
  protected static $domains = array();

  // This is the key from Monotype's Drupal module. TODO: Get our own.
  protected static $apiKey = 'cc2b2294-4ace-4e79-81de-15275642f7fd35880';

  protected $publicKey;
  protected $privateKey;

  /**
   * Returns an instance of the FontManagementMonotype class.  Only one instance
   * should be created so we can cache results to reduce the number of
   * queries.
   *
   * @return {FontManagementMonotype}
   *   The FontManagementMonotype instance.
   */
  public static function getInstance() {
    if (empty(FontManagementMonotype::$instance)) {
      FontManagementMonotype::$instance = new FontManagementMonotype();
    }
    return FontManagementMonotype::$instance;
  }

  /**
   * Sets credentials for use in the authentication header.
   *
   * @param {String} $token
   *   The authentication token associated with the monotype account.
   */
  public function authenticate($token) {
    list($this->publicKey, $this->privateKey) = explode('--', $token);
  }

  /**
   * Indicates whether the account associated with the specificed token is a free account.
   *
   * @param {String} $token
   *   The authentication token associated with the monotype account.
   *
   * @return {boolean}
   *   TRUE if the specified account is a free account; FALSE otherwise.
   */
  public function isFreeAccount($token) {
    $account = $this->getProjectList($token);
    return (strtolower((string)$account->UserRole) == 'free');
  }

  /**
   * Returns the list of valid projects for the account associated with the specified token.
   *
   * @param {String} $token
   *   The authentication token associated with the monotype account.
   *
   * @return
   *   The response of the projects call if successful.  Otherwise an
   *   exception will be thrown.
   */
  public function getProjectList($token) {
    $project = FontManagementMonotype::$projects[$token];
    if (empty($project)) {
      $this->authenticate($token);
      $project = $this->makeRequest('GET', 'Projects/');
      $this->checkResult($project);
      FontManagementMonotype::$projects[$token] = $project;
    }
    return $project;
  }

  /**
   * Returns the list of fonts for the project associated with the specified project identifier
   *
   * @param {String} $token
   *   The authentication token associated with the monotype account.
   * @param {String} projectKey
   *   The identifier of the project.
   *
   * @return
   *   The response of the fonts call if successful.  Otherwise an
   *   exception will be thrown.
   */
  public function getFontList($token, $projectKey) {
    $fonts = FontManagementMonotype::$fonts[$token .'|' .$projectKey];
    if (empty($fonts)) {
      $this->authenticate($token);
      $args = array(
        'wfspid' => $projectKey,
      );
      $path = "Fonts/?" . $this->construct_arguments($args);
      try {
        $fonts = $this->makeRequest('GET', $path);
        $this->checkResult($fonts);
      }
      catch (FontManagementMonotypeInvalidResponseException $e) {
        // Getting an invalid response when the project id isn't valid.
        throw new FontManagementMonotypeUnknownProjectException($e->getMessage());
      }
      FontManagementMonotype::$fonts[$token .'|' .$projectKey] = $fonts;
    }
    return $fonts;
  }

  /**
   * Returns the list of domains associated with the specified project identifier.
   *
   * @param {String} $token
   *   The authentication token associated with the monotype account.
   * @param {String} projectKey
   *   The identifier of the project.
   *
   * @return
   *   The response of the domains call if successful.  Otherwise an
   *   exception will be thrown.
   */
  public function getDomainsList($token, $projectKey) {
    $domains = Monotype::$domains[$token .'|' .$projectKey];
    if (empty($domains)) {
      $this->authenticate($token);
      try {
        $domains = $this->makeRequest('GET', 'Domains/?wfspid=' . $projectKey);
        $this->checkResult($domains);
      }
      catch (FontManagementMonotypeInvalidResponseException $e) {
        // Getting an invalid response when the project key isn't valid.
        throw new FontManagementMonotypeUnknownProjectException($e->getMessage());
      }
      FontManagementMonotype::$domains[$token .'|' .$projectKey] = $domains;
    }
    return $domains;
  }

  /**
   * Sends the request to the Monotype server.
   *
   * The request is sent as an HTTP POST, in accordance with their API.
   *
   * @param {Array} $args
   *   The arguments for the request.  These arguments describe what
   *   type of request is being sent and provide any information
   *   required to complete the request.
   *
   * @return
   *   An object that represents the result of the request.  If the
   *   result is poorly formed, an exception is thrown.
   */
  protected function makeRequest($method, $path, $args = array()) {
    $requestUri = '/rest/xml/' . $path;

    $curl_options = array(
      CURLOPT_URL => FontManagementMonotype::$API_URI . $requestUri,
      CURLOPT_HEADER => 0,
      CURLOPT_CUSTOMREQUEST => $method,
      CURLOPT_RETURNTRANSFER => TRUE,
    );

    // Add headers for authorization.
    $authHeader = $this->publicKey . ":" . base64_encode(hash_hmac('md5', $this->publicKey . "|" . $requestUri, $this->privateKey, TRUE));
    $curl_options[CURLOPT_HTTPHEADER] = array(
      "Authorization: " . urlencode($authHeader),
      "AppKey: " . FontManagementMonotype::$apiKey,
    );

    // Add any POST fields.
    $fields = $this->construct_arguments($args);
    if (!empty($fields)) {
      $curl_options[CURLOPT_POSTFIELDS] = $fields;
      $curl_options[CURLOPT_POST] = TRUE;
    }

    // Retrieve data.
    $ch = curl_init();
    curl_setopt_array($ch, $curl_options);

    $output = curl_exec($ch);
    curl_close($ch);

    try {
      $result = new SimpleXmlElement($output);
    }
    catch (Exception $e) {
      throw new FontManagementMonotypeInvalidResponseException($e->getMessage());
    }
    return $result;
  }

  /**
   * Converts the specified argument array into a parameter string suitable for a POST request.
   *
   * @param {Array} $args
   *   The arguments to pass through the request.
   *
   * @return
   *   The string version of the arguments.
   */
  protected function construct_arguments($args) {
    $output = '';
    $keys = array_keys($args);
    $size = count($keys);

    for ($i = 0; $i < $size; $i++) {
      if ($i > 0) {
        $output .= '&';
      }
      $output .= $keys[$i] .'=' .urlencode($args[$keys[$i]]);
    }
    return $output;
  }

  /**
   * Checks the result of a request to determine if it represents a valid response or an error condition.
   *
   * @param {Object} $result
   *   The object representing the response.
   *
   * @return
   *   The object if the response represents useful data.  Otherwise
   *   an appropriate exception will be thrown.
   */
  protected function checkResult($result) {
    if (empty($result) || !isset($result->Message)) {
      throw new FontManagementMonotypeInvalidResponseException('Bad response');
    }
    switch ($result->Message) {
    case 'Success':
      return;

    case 'Invalid Authentication':
      throw new FontManagementMonotypeAuthenticationException($result->Message);

    case 'Project List could-not be retrieved':
      throw new FontManagementMonotypeProjectListException($result->Message);

    case 'Failed to retrieve the font information':
      throw new FontManagementMonotypeFontListException($result->Message);

    default:
      throw new FontManagementMonotypeException($result->Message);
    }
  }
}

/**
 * This is the base class of all exceptions thrown by the Monotype connector.
 */
class FontManagementMonotypeException extends Exception {}

/**
 * This exception indicates the response was not in a valid format, and thus unreadable.
 */
class FontManagementMonotypeInvalidResponseException extends FontManagementMonotypeException {}

/**
 * This exception indicates the authentication failed.
 */
class FontManagementMonotypeAuthenticationException extends FontManagementMonotypeException {}

/**
 * This exception indicates the project list could not be retrieved.
 */
class FontManagementMonotypeProjectListException extends FontManagementMonotypeException {}

/**
 * This exception indicates the project key is unknown.
 */
class FontManagementMonotypeUnknownProjectException extends FontManagementMonotypeException {}

/**
 * This exception indicates the font list could not be retrieved.
 */
class FontManagementMonotypeFontListException extends FontManagementMonotypeException {}

/**
 * This exception indicates the domain list could not be retrieved.
 */
class FontManagementMonotypeDomainsListException extends FontManagementMonotypeException {}
